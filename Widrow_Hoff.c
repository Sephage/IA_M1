/*
 * A = 1
 * C = 0
 * threshold = 0.5
 * */
#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <unistd.h>
#include <time.h>
#include <math.h>

int number_neurons = 20;
const float threshold = 0.5;

int load_file(const char* path, int* neurons){
    int i;
    FILE* file = NULL;

    file = fopen(path, "r");
    if(file == NULL){
        return 1;
    }
    i=0;
    while((neurons[i] = getc(file)-48) != EOF && i<number_neurons){ 
        i++;
    }
    fclose(file);
    return 0;
}

void test_recognition(const char* file, const int letter, float* weights, 
        int* input_vector, int* input_vector2){
    /*variables Initialization*/
    int output_vector;
    int nb_test = 100;
    int k,l,j,i;
    int change[20] = {0,0,0,0,0,0,0,0,0,0};
    int errors[40];
    int error;
    float potential;
    /*
     * For each N between 0;10
     * change N cases
     * see if the NN recognize
     * save if yes or no
     * repeat 5 times each loop
     */
    for(i=0 ; i<40; i++){
        errors[i] = 0;
    }
    srand(time(NULL));
    if(load_file(file,input_vector)){
        printf("Error load file\n");
        exit(0);
    }
    printf("I will try to recognize %s\n", file);

    for(j=0;j<20;j++){
        input_vector2[j] = input_vector[j];
    }
    /*
     * j= nb case changed
     * i= number of test
     */
    for(j=0 ; j<16 ; j++){
        for(i=0;i<nb_test;i++){
            for(k=0 ; k<=j ; k++){
                /*Get random case to change*/
                int can_change = 0;
                while(!can_change){
                    change[k] = rand()%20;
                    can_change = 1;
                    int m;
                    for(m=0 ; m<k ; m++){
                        if(change[k] == change[m]) can_change=0;
                    }
                }
                input_vector[change[k]] = input_vector[change[k]]*-1 + 1;
            }

            /*Calculate pot*/
            potential = 0.0;
            for(k=0 ; k<number_neurons ; k++){
                potential += weights[k]*input_vector[k];
            }
            output_vector = (potential >= threshold);
            //printf("%s %d %f %f\n", file, output_vector, potential, threshold);
            /*Check if letter is recognize or not*/
            if(output_vector != letter){
                errors[j]++;
            }
            for(k=0 ; k<=j ; k++){
                input_vector[change[k]] = input_vector[change[k]]*-1 + 1;
            }
        }
        printf("Nb cases which are changes = %d,  error = %d/%d\n",j+1, errors[j], nb_test);
    }
}

int learning(float* weights, const char** files, const int* result, int number_file, int* input_vector,
        int* error, float* pot){
    int i,j;
    float potential;
    int output_vector;
    for(i=0;i<number_file;i++){
        if(load_file(files[i], input_vector)){
            printf("%s load file problem\n", files[i]);
            return 1;
        }
        /*Calcul of the potential*/
        potential = 0.0;
        for(j=0;j<number_neurons;j++){
            potential += weights[j]*input_vector[j];
        }
        output_vector = (potential >= threshold);
        if(output_vector != result[i]){
            *error = *error + 1;
        }
        for(j=0;j<number_neurons;j++){
            weights[j] += 0.05*(result[i]-potential)*input_vector[j];
        }
        pot[i] = potential;
    }
}

int main(int argc, char** argv){
    float* weights;
    int* input_vector;
    int* input_vector2;
    float potential;
    int* errors;
    const int nb_files = 2;
    float pot[2];

    int error = 1;
    int i=0;
    int j=0;

    const int A = 1;
    const int C = 0;

    const char* name_files[2] = {"A.txt", "C.txt"};
    const int result[2] = {A,C};

    input_vector = malloc(sizeof(int)*number_neurons);
    input_vector2 = malloc(sizeof(int)*number_neurons);
    weights = malloc(sizeof(float)*number_neurons);
    errors = malloc(sizeof(int)*60);

    srand48(time(NULL));
    for(j=0 ; j<number_neurons ; j++){
        weights[j] = drand48()/20;
    }
    for(j=0 ; j<60 ; j++){
        errors[j] = -2;
    }

    /*Learning*/
    float disp = 1;
    while(disp > 0.00000000001){
        error = 0;
        learning(weights, name_files, result, nb_files, input_vector, &error, pot);
        errors[i] = error;
        if(i==59){} 
        else i++;
        disp = powf(result[0] - pot[0],2) + powf(result[1] - pot[1],2);
        //printf("disp = %f, pot[0] = %f, pot[1] = %f\n", disp, pot[0], pot[1]);
    }

    for(j=0;j<i;j++){
        printf("iteration %d nb error %d\n", j, errors[j]);
    }
    printf("\n");

    test_recognition("A.txt", A, weights, input_vector, input_vector2);
    test_recognition("C.txt", C, weights, input_vector, input_vector2);
    return 0;
}
