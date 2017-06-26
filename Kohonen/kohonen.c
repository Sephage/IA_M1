#include "kohonen.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <float.h>

#include <math.h>


static void use(char* prog){ 
    fprintf(stderr, "usage: %s [-n:r]\n", prog);
    exit(1);
}

void init_neurons(){
    int i;
    for(i=0 ; i<nb_neurons ; i++){
        neurons[i].x = cos(2*M_PI*i/nb_neurons)/2;
        neurons[i].y = sin(2*M_PI*i/nb_neurons)/2;
    }
}

int compute_activity(float* weight_map){
    int i;
    float min = FLT_MAX;
    int ind;

    for(i=0 ; i<nb_neurons ; i++){
        if(weight_map[i] < min){
            min = weight_map[i];
            ind = i;
        }
    }
    return ind;
}

int update_weight(Coordinate* selected){
    int i;
    for(i=0 ; i<nb_neurons ; i++){
        weight_map[i] = powf(neurons[i].x - selected->x, 2) + powf(neurons[i].y - selected->y, 2);
    }
}

/*
 * Chaque poids est la distance euclidienne entre un neurone et le WDS
 */
void init_weight(float** weight_map){
    weight_map = malloc(nb_neurons * sizeof(float));
}

double neighborhood_function(float dist, float var){
    int a = 1.84;
    if(dist == 0) return 1;
    return var/2 * (sin(a*dist)/a*dist) * exp(-powf(dist,2)/powf(var,2));
}

double learning_rate(int cpt){
    if(cpt > 1000000) return 0;
    return 1.0 - (cpt/10000000);
}

void update_neurons(Coordinate selected, int cpt, int win){
    int i;
    float s = 1.9;
    int iteration = 1500000;
    float epsilon = 0.02;
    for(i=0 ; i<nb_neurons ; i++){
        neurons[i].x += epsilon * (selected.x - neurons[i].x) * neighborhood_function(abs(win - i), s - (cpt/iteration));
        neurons[i].y += epsilon * (selected.y - neurons[i].y) * neighborhood_function(abs(win - i), s - (cpt/iteration));
    }
}
