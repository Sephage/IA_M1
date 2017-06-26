#include "multi.h"
#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <unistd.h>
#include <time.h>
#include <math.h>

int SIZE = 25;

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

void init_file(char** files, char* dir, int nb_files){
    int i;
    for(i=0 ; i<nb_files ; i++){
        sprintf(files[i], "%s%c.txt", dir, i+65);
    }
}

int learning(Network *network, double** outputs_desired, int* error){
    int i,j,k, l;
    char file_name[100];
    double error_rate = 1.0;
    const double max_error = 0.0005;

    /*Add loop until condition reached we continue to learn*/
    while(error_rate > max_error){
        error_rate = 0.0;
        for(i=0;i<nb_file;i++){
            sprintf(file_name, "../lettre_5*5/%c.txt", i+65); 
            if(load_file(file_name, &(network->layers[0]))){
                printf("%s problem to load file\n", file_name);
            }

            //step 1
            compute_output(network);


            //step 2 
            double delta_out[network->layers[network->nb_layers-1].number_neurons];
            for(j=0 ; j<network->layers[network->nb_layers-1].number_neurons ; j++){
                double output = network->layers[network->nb_layers-1].neurons[j].input;
                delta_out[j] = output * (1-output) * (outputs_desired[i][j] - output);
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
        }
    }
    return 0;
}

void test_recognition(char** files, Network* network){
    int i, j, k, l;
    int change[22] = {-1,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int is_error;
    int is_correct;
    int hard_error; //number of error when the charac isn't recognize
    int soft_error; //When the character is recognize but another char is also recognize
    int C_NB_TEST = 100;
    double input_vector[network->layers[0].number_neurons];

    FILE* file[26];
    char name_save[50];

    srand(time(NULL));
    for(i=0 ; i<26 ; i++){
        sprintf(name_save, "%s%c.txt", SAVE_DIR, i+65);
        file[i] = fopen(name_save, "w+");
        if(file == NULL){
            printf("%s\n", name_save);
            perror("Saving file not open\n");
            exit(1);
        }
    }
    for(i=0 ; i<26 ; i++){
        printf("\n%c\n", i+65);
        if(load_file(files[i], &(network->layers[0]))){
            printf("%s problem to load file\n", files[i]);
        }

        for(l=0 ; l<network->layers[0].number_neurons ; l++){
            input_vector[l] = network->layers[0].neurons[l].input;
        }

        for(j=0 ; j<20 ; j++){

            hard_error = 0;
            soft_error = 0;
            for(k=0 ; k<C_NB_TEST ; k++){
                is_correct = 0;
                is_error = 0;
                for(l=0 ; l <= j ; l++){
                    int can_change = 0;
                    while(!can_change){
                        change[l] = rand()%25;
                        can_change = 1;
                        int m;
                        for(m=0 ; m<l ; m++){
                            if(change[l] == change[m]) can_change = 0;
                        }
                    }
                    network->layers[0].neurons[change[l]].input = network->layers[0].neurons[change[l]].input*-1 + 1;
                }
                compute_output(network);
                for(l=0 ; l<network->layers[network->nb_layers-1].number_neurons ; l++){
                    if(network->layers[network->nb_layers-1].neurons[l].input >= 0.5) {
                        if(l == i){
                           is_correct = 1;
                        }
                        else{ 
                            is_error = 1;
                            //printf("(%d, %d) %c = %.2f\n", j, k, l+65, network->layers[network->nb_layers-1].neurons[l].input);
                        }
                    }
                }
                if(is_correct == 0) {
                    hard_error++;
                }
                else{
                    if(is_error == 1 && is_correct == 1) 
                        soft_error++;
                }

                for(l=0 ; l<network->layers[0].number_neurons ; l++){
                    network->layers[0].neurons[l].input = input_vector[l];
                }
            }
            
            fprintf(file[i], "%d : (%d;%d) ", j+1, soft_error, hard_error);
        }
    }
    for(i=0 ; i<26 ; i++){
        fclose(file[i]);
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
int main(int argc, char** argv){
    int i,j;
    double** outputs_desired;
    char *files[nb_file];
    Network network;

    network.nb_layers = 3;
    network.layers = malloc(network.nb_layers*sizeof(Layer));

    network.layers[0].number_neurons = 25;
    network.layers[1].number_neurons = 20;
    network.layers[2].number_neurons = 26;

    for(i=0;i<network.nb_layers;i++){
        network.layers[i].neurons = malloc(network.layers[i].number_neurons*sizeof(Neuron));
    }

    for(i=0 ; i<network.nb_layers-1 ; i++){
        for(j=0 ; j<network.layers[i].number_neurons ; j++){
            network.layers[i].neurons[j].weights = malloc(network.layers[i+1].number_neurons*sizeof(double));
        }
    }

    outputs_desired = malloc(nb_file * sizeof(double*));
    for(i=0 ; i<nb_file ; i++){
        outputs_desired[i] = malloc(number_out_neurons * sizeof(double));
    }

    for(i=0 ; i<nb_file ; i++){
        files[i]=malloc(19*sizeof(char));
    }

    init_weights(&network);
    init_output_desired(outputs_desired);
    init_file(files, "../lettre_5*5/",26);

    learning(&network, outputs_desired, NULL);

    test_recognition(files, &network);

    /**************************************/
    /************ Free ********************/
    /**************************************/
    for(i=0 ; i<nb_file ; i++){
        free(files[i]);
    }
    for(i=0 ; i<network.nb_layers-1 ; i++){
        for(j=0 ; j<network.layers[i].number_neurons ; j++){
            free(network.layers[i].neurons[j].weights);
        }
    }
    /*printf("Free neurons\n");
    for(i=0;i<network.nb_layers;i++){
        free(network.layers[i].neurons);
    }*/
    free(network.layers);

    for(i=0 ; i<nb_file ; i++){
        free(outputs_desired[i]);
    }
    free(outputs_desired);
    return 0;
}
