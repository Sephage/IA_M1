/*
 * A = 1
 * C = 0
 * treshold = 0.5
 * */
#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <unistd.h>
#include <time.h>

int load_file(const char* path, int* neurons){
    int i;
    FILE* file = NULL;

    file = fopen(path, "r");
    if(file == NULL){
        return 1;
    }
    i=0;
    while((neurons[i] = getc(file)-48) != EOF && i<20){ //Change this line if we change number of neurons
        i++;
    }
    fclose(file);
    return 0;
}

int main(int argc, char** argv){
    float* weights;
    int* input_vector;
    int* input_vector2;
    int number_neurons = 20;
    int output_vector;
    float potential;
    int* errors;

    int error = 1;
    int i=0;
    int j=0;
    const int treshold = 0.5;

    const int A = 1;
    const int C = 0;


    input_vector = malloc(sizeof(int)*number_neurons);
    input_vector2 = malloc(sizeof(int)*number_neurons);
    weights = malloc(sizeof(float)*number_neurons);
    errors = malloc(sizeof(int)*40);

    srand48(time(NULL));
    for(j=0 ; j<number_neurons ; j++){
        weights[j] = drand48()/20;
    }
    int m;
    /*Learning*/
   for(m=0 ; m<15 ; m++){
        error = 0;
        /*Load A.txt into input_vector*/
        if(load_file("./A.txt", input_vector)){
            printf("A load file problem\n");
            exit(1);
        }
        /*Calcul of the potential*/
        potential = 0;
        for(j=0;j<number_neurons;j++){
            potential += weights[j]*input_vector[j];
        }
        output_vector = (potential >= treshold);

        if(output_vector != A){
            error++;
            for(j=0;j<number_neurons;j++){
                weights[j] += 0.05*(A-potential)*input_vector[j];
            }
        }

        /*Load C.txt*/
        if(load_file("./C.txt", input_vector)){
            printf("C load file problem\n"); 
            exit(1);
        }

        potential = 0;
        for(j=0;j<number_neurons;j++){
            potential += weights[j]*input_vector[j];
        }
        output_vector = (potential >= treshold);

        if(output_vector != C){
            error++;
            for(j=0;j<number_neurons;j++){
                weights[j] += 0.05*(C-potential)*input_vector[j];
            }
        }
        errors[i] = error;
        if(i==39) i=0;
        else i++;
    }

    for(j=0;j<i;j++){
        printf("iteration %d nb error %d\n", j, errors[j]);
    }
    printf("\n");

    /*variables Initialization*/
    int l;
    int change[20] = {0,0,0,0,0,0,0,0,0,0};
    for(int l=0 ; l<20 ; l++){
        errors[l] = -1;
    }
    /*
     * For each N between 0;10
     * change N cases
     * see if the NN recognize
     * save if yes or no
     * repeat 5 times each loop
     */
    srand(time(NULL));
    int k;
    char* name_file = "A.txt";
    if(load_file(name_file,input_vector)){
        printf("Error load file\n");
        exit(0);
    }
    printf("I will try to recognize %s\n", name_file);

    for(j=0;j<20;j++){
        input_vector2[j] = input_vector[j];
    }
    for(j=0 ; j<10 ; j++){
        error = 0;
        for(i=0 ; i<10 ; i++){
            for(k=0;k<=j;k++){
                /*Get j unique random cases to change*/
                int can_change = 0;
                while(!can_change){
                    change[k] = rand()%20;
                    can_change = 1;
                    int m;
                    for(m=0;m<k;m++){
                        if(change[k] == change[m]) can_change=0;
                    }
                }
            }
            for(k=0;k<=j;k++){
                input_vector2[change[k]] = input_vector[change[k]]*-1 + 1;
            }

            /*Calculate pot*/
            potential = 0;
            for(k=0;k<number_neurons;k++){
                potential += weights[k]*input_vector2[k];
            }
            output_vector = (potential >= treshold);
            /*Check if letter is recognize or not*/
            if(output_vector != A){
                int y,p;
                for(y=0;y<5;y++){
                    for(p=0;p<4;p++){
                        printf("%d",input_vector2[p+4*y]);
                    }
                    printf("\n");
                }
                printf("Error\n");
                error++;
            }

            for(k=0;k<l;k++){
                input_vector2[change[k]] = input_vector[change[k]];
            }
        }
        errors[j] = error;
    }
    for(j=0;j<10;j++){
        printf("changes = %d,  error = %d/10\n",(j+1), errors[j]);
    }
    return 0;
}
