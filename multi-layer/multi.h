const double threshold = 0.5;
const double eta = 0.2;
const int nb_file = 26;
const int number_out_neurons = 26;

const char* SAVE_DIR = "./results/";


typedef struct{
    double* weights;
    double input;
}Neuron;

typedef struct{
    int number_neurons;
    Neuron *neurons;
}Layer;

typedef struct{
    int nb_layers;
    Layer* layers;
}Network;

/********************************************/
/********************************************/

/*
 * Fill the layer input_layer with the content of one file
 * */
int load_file(const char* path, Layer* input_layer);

/*
 * Initialize wights randomly
 * */
void init_weights(Network *network);

/*
 * Initialize what the result that we want after each loop of learning
 * */
void init_output_desired(double** outputs_desired);

/*
 * Fill files with the path of different file that contain neurons value
 * */
void init_file(char** files, char *dir, int nb_files);

int learning(Network *network, double** outputs_desired, int* error);
void test_recognition(char** files, Network* network);

int compute_output(Network *network);
