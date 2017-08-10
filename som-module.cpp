#include"som-module.hpp"

void SOMLAYER::Print_Weight(int index) {
    for(int i=0;i<number_of_input_values;i++) {
        std::cout<<weight[index][i]<<std::endl;
    }
    return;
}

SOMLAYER::SOMLAYER(int input_dimension, int output_dimension) {

    // Initialize data structure
    weight = new double*[output_dimension];
    for(int i=0;i<output_dimension;i++) {
        weight[i] = new double[input_dimension];
    }
    number_of_input_values = input_dimension;
    number_of_output_neurons = output_dimension;
    current_winner = -1;
    current_input = NULL;
    learning_rate = 0.3;

    // Initialize weights
    for(int i=0;i<output_dimension;i++) {
        for(int j=0;j<input_dimension;j++) {
            weight[i][j] = my_rand()-RAND_MAX/2;
        } 
        Vector_Normalization(weight[i],input_dimension);
        // Used to check weigts initialization
        // Print_Weight(i);
    }    
    return;
}

SOMLAYER::SOMLAYER(std::ifstream *read_file) {
    (*read_file)>>number_of_input_values;
    (*read_file)>>number_of_output_neurons;
    (*read_file)>>learning_rate;
    weight = new double*[number_of_output_neurons];
    for(int i=0;i<number_of_output_neurons;i++) {
        weight[i] = new double[number_of_input_values];
    }
    current_winner = -1;
    current_input = NULL;
    learning_rate = 0.3;
    for(int i=0;i<number_of_output_neurons;i++) {
        for(int j=0;j<number_of_input_values;j++) {
            (*read_file)>>weight[i][j];
        }
    }
    return;
}

double SOMLAYER::Inner_Product(double *v1, double *v2, int dim) {
    double result = 0.0;
    for(int i=0;i<dim;i++) {
        result += v1[i]*v2[i];
    }
    return result;
}

SOMLAYER::~SOMLAYER() {
    for(int i=0;i<number_of_output_neurons;i++) {
        delete [] weight[i];        
    }
    delete [] weight;
    return;
}

void SOMLAYER::Save_SOM(std::ofstream *save_file) {
    (*save_file)<<number_of_input_values<<" ";
    (*save_file)<<number_of_output_neurons<<" ";
    (*save_file)<<learning_rate<<std::endl;
    for(int i=0;i<number_of_output_neurons;i++) {
        for(int j=0;j<number_of_input_values;j++) {
            (*save_file)<<weight[i][j]<<" ";
        }
        (*save_file)<<std::endl;
    }
    return;
}

int SOMLAYER::Determine_Winner() {
    if(!current_input) {
        std::cout<<"No input found"<<std::endl;
        throw -1;
    }
    double max_value = 0.0;
    int winner = 0;
    for(int i=0;i<number_of_output_neurons;i++) {
        double current_value = Inner_Product(current_input,weight[i],number_of_input_values);
        if(max_value<current_value) {
            max_value = current_value;
            winner = i;
        }
    }
    winner_value = max_value;
    return winner;
}

void SOMLAYER::Self_Reorganize() {
    for(int i=0;i<number_of_input_values;i++) {
        weight[current_winner][i] += learning_rate*(current_input[i]-weight[current_winner][i]); 
    }
    Vector_Normalization(weight[current_winner],number_of_input_values);
}

int SOMLAYER::Send_Input_To_SOMLAYER(double *input_vector) {

    //Print_Vector(input_vector, number_of_input_values);
    
    current_input = input_vector;
    current_winner = Determine_Winner();
    std::cout<<"    winning neuron id: "<<current_winner<<std::endl;
    std::cout<<"    winning value: "<<winner_value<<std::endl;
    Self_Reorganize();

    // Usually these input data is temporary, reset to null to avoid dangling pointer
    current_input=NULL;
    return current_winner;
}