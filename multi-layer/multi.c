#include "multi.h"

#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <unistd.h>
#include <time.h>
#include <math.h>

int load_file(const char* pathName, Layer* input_layer){
    int i;
    FILE* file = NULL;

    file = fopen(pathName, "r");
    if(file == NULL) return 1;

    i=0;
    while((input_layer->neurons[i].input = getc(file)-48) != EOF && i< input_layer->number_neurons){
        i++;
    }
    fclose(file);
    return 0;
}

void init_weights(Network *network){
    int i,j,k;
    srand48(time(NULL));
    for(i=0 ; i<(network->nb_layers-1) ; i++){
        for(j=0 ; j<network->layers[i].number_neurons ; j++){
            for(k=0 ; k<network->layers[i+1].number_neurons ; k++){
                network->layers[i].neurons[j].weights[k] = drand48()/20;
            }
        }
    }
}

int learning(Network *network, double** outputs_desired, int number_file, int* error, double* pot){
    int i,j,k, l;
    char file_name[100];
    double error_rate = 1.0;
    const double max_error = 0.0005;

    /*Add loop until condition reached we continue to learn*/
    while(error_rate > max_error){
        error_rate = 0.0;
        for(i=0;i<26;i++){
            sprintf(file_name, "../lettre_5*5/%c.txt", i+65); 
            if(load_file(file_name, &(network->layers[0]))){
                printf("%s problem to load file\n", file_name);
            }

            //step 1
            /*for(j=1 ; j<network->nb_layers ; j++){
                for(k=0 ; k<network->layers[j].number_neurons ;k++){
                    double neuron_input = 0.0;
                    network->layers[j].neurons[k].input = 0.0;
                    for(l=0 ; l<network->layers[j-1].number_neurons ; l++){
                        Neuron neur_prev_lay = network->layers[j-1].neurons[l];
                        neuron_input += neur_prev_lay.input * neur_prev_lay.weights[k];
                    }
                    network->layers[j].neurons[k].input = 1/(1+exp(-neuron_input));
                }
            }*/

            compute_output(network);


            //step 2 
            double delta_out[network->layers[network->nb_layers-1].number_neurons];
            for(j=0 ; j<network->layers[network->nb_layers-1].number_neurons ; j++){
                double output = network->layers[network->nb_layers-1].neurons[j].input;
                delta_out[j] = output * (1-output) * (outputs_desired[i][j] - output);
                if(isnan(delta_out[j])) printf("ERROR delta_out[%d]\n",j);
            }

            //step 3
            double* delta_hid[network->nb_layers-2];
            for(j=1 ; j<network->nb_layers-1 ; j++){
                delta_hid[j-1] = malloc(network->layers[j].number_neurons * sizeof(double));
            }
            for(j=network->nb_layers-2 ; j > 0 ; j--){
                int m = j-1;
                for(k=0 ; k<network->layers[j].number_neurons ; k++){
                    double output = network->layers[j].neurons[k].input;
                    double sum = 0.00;
                    for(l=0 ; l<network->layers[j+1].number_neurons ; l++){
                        if(j == network->nb_layers-2)
                            //delta_hid[m][k] += network->layers[j].neurons[k].weights[l] * delta_out[l];
                            sum += network->layers[j].neurons[k].weights[l] * delta_out[l];

                        else
                            //delta_hid[m][k] += network->layers[j].neurons[k].weights[l] * delta_hid[j][l];
                            sum += network->layers[j].neurons[k].weights[l] * delta_hid[j][l];
                    }
                    delta_hid[m][k] = output * (1-output) * sum;
                }
            }

            //step 4
            for(j=0 ; j < network->nb_layers-1 ; j++){
                for(k=0 ; k<network->layers[j].number_neurons ; k++){
                    Neuron* neuron = &(network->layers[j].neurons[k]);
                    for(l=0 ; l<network->layers[j+1].number_neurons ; l++){
                        if(j == network->nb_layers-2)
                            neuron->weights[l] += eta*(delta_out[l] * neuron->input);
                        else
                            neuron->weights[l] += eta*(delta_hid[j][l] * neuron->input);
                    }
                } 
            } 

            for(j = 0; j<network->layers[network->nb_layers-1].number_neurons ; j++){
                error_rate += pow(outputs_desired[i][j] - network->layers[network->nb_layers-1].neurons[j].input,2.0);
            }
            for(j=0 ; j<network->nb_layers-2 ; j++){
                free(delta_hid[j]);
            }
        }
    }
}

int compute_output(Network *network){
    int j,k,l;

    for(j=1 ; j<network->nb_layers ; j++){
        /*For all neurons of all layers except the first one we calculate the input of the neuron*/
        for(k=0 ; k<network->layers[j].number_neurons ;k++){
            double neuron_input = 0.0;
            network->layers[j].neurons[k].input = 0.0;
            for(l=0 ; l<network->layers[j-1].number_neurons ; l++){
                Neuron neur_prev_lay = network->layers[j-1].neurons[l];
                neuron_input += neur_prev_lay.input * neur_prev_lay.weights[k];
                
            }
            
            network->layers[j].neurons[k].input = 1/(1+exp(-neuron_input));
            if(isnan(exp(-neuron_input))){
                printf("ERROR neuron_input %lf\n", neuron_input); 
                exit(2);
            }
            if(isnan(1/1+exp(-neuron_input))){
                printf("ERROR 2\n");
                exit(3);
            }
        }
    }
    return 0;
}

void test_recognition(const char** files, Network* network, const double* input_vector){
    int i,j;
    int change[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int error_strict; //number of error when the charac isn't recognize
    int error_soft; //When the character is recognize but other is also recognize


    for(i=0 ; i<26 ; i++){
        if(load_file(files[i], &(network->layers[0]))){
            printf("%s problem to load file\n", files[i]);
        }

        for(i=0 ; i<40 ; i++){
            compute_output(network);
            for(i=0 ; i<26 ; i++){
                if(network->layers[2].neurons[i].input>=0.5) 
                    printf("%c = %.2f\n", i+65, network->layers[2].neurons[i].input);
            }
            printf("\n");
        }
    }
}

void init_output_desired(double** outputs_desired){
    int i,j;
    for(i=0 ; i<nb_file ; i++){
        for(j=0 ; j<number_out_neurons ; j++){
            if(i == j) outputs_desired[i][j] = 1;
            else outputs_desired[i][j] = 0;
        }
    }
}

/*
 *
 * */
int main(int argc, char* argv){
    int i,j;
    double** outputs_desired;
    Network network;

    network.nb_layers = 3;
    //Malloc du nombre de couche
    network.layers = malloc(network.nb_layers*sizeof(Layer));

    network.layers[0].number_neurons = 25;
    network.layers[1].number_neurons = 50;
    network.layers[2].number_neurons = 26;

    //malloc du nombre de neurone par couche
    for(i=0;i<network.nb_layers;i++){
        network.layers[i].neurons = malloc(network.layers[i].number_neurons*sizeof(Neuron));
    }

    //Malloc de chaque vecteur poids de chaque neurone
    for(i=0 ; i<network.nb_layers-1 ; i++){
        for(j=0 ; j<network.layers[i].number_neurons ; j++){
            network.layers[i].neurons[j].weights = malloc(network.layers[i+1].number_neurons*sizeof(double));
        }
    }

    outputs_desired = malloc(nb_file * sizeof(double*));
    for(i=0 ; i<nb_file ; i++){
        outputs_desired[i] = malloc(number_out_neurons * sizeof(double));
    }

    init_weights(&network);
    init_output_desired(outputs_desired);

    learning(&network, outputs_desired, 26, NULL, NULL);
    printf("\nEnd learning\n"); 
    for(i=0 ; i<26 ; i++){
        printf("%f ", network.layers[2].neurons[i].input);
    }
    printf("\n");
    //test_recognition("../lettre_4*5/E_trans.txt", &network);
    return 0;
}
