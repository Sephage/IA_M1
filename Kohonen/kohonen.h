#ifndef KOHONEN_H
#define KOHONEN_H

#define NB_NEURONS 20
#define WIDTH 20
#define HEIGHT 20


typedef struct coord{
    float x;
    float y;
}Coordinate;

float *weight_map;
Coordinate *neurons;
Coordinate* input;
int nb_neurons;

static void use(char* prog);
int compute_activity();
int update_weight(Coordinate* selected);
void init_weight();
void init_neurons();

/*
 * return the number of inputs
 * */
int init_input(Coordinate** input, char* filename);

double neighborhood_function(float dist, float var);
double learning_rate(int cpt);
void update_neurons(Coordinate selected, int cpt, int win);


#endif
