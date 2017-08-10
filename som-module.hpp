#include "algo.hpp"

class SOMLAYER {
    
    public:

        SOMLAYER(int input_dimension, int output_dimension);
        SOMLAYER(std::ifstream *read_file);
        ~SOMLAYER();

        int Send_Input_To_SOMLAYER(double *input_vector);    

        void Save_SOM(std::ofstream *save_file);    
        
        // subword_dimension
        int number_of_input_values;
        // current winner
        int current_winner;

    protected:

        void Self_Reorganize();
        void Print_Weight(int index);
        double Inner_Product(double *v1, double *v2, int dim);
        int Determine_Winner();

    private:

        // A two-dim array, first index is neuron index with size number_of_output_neurons
        // second index is weights owned by a single neuron 
        double **weight;
        int number_of_output_neurons;

        // current inner product value produced with current winner weigths vector and current input
        double winner_value;
        double *current_input;

        // should be a value between [0,1]
        double learning_rate;

};