//const int number_neurons = 20;
const double threshold = 0.5;
const double eta = 0.2;
const int nb_file = 26;
const int number_out_neurons = 26;


typedef struct{
    double* weights;
    double input;
    //double output;
}Neuron;

typedef struct{
    int number_neurons;
    Neuron *neurons;
}Layer;

typedef struct{
    int nb_layers;
    Layer* layers;
}Network;

int load_file(const char* path, Layer* input_layer);
//void test_recognition(const char* file, const int letter, Network *network);
int learning(Network *network, double** outputs_desired, int number_file, int* error, double* pot);
void init_weights(Network *network);
void init_output_desired(double** outputs_desired);
//void test_recognition(char* file, Network* network);
int compute_output(Network *network);
void test_recognition(const char** files, Network* network, const double* input_vector);

