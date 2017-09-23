#pragma once

#include "som-module.hpp"

class RESULT_COLLECTOR {

    public:

        RESULT_COLLECTOR();
        ~RESULT_COLLECTOR();

        void Add_New_Result(std::string name);
        void Add_Raw_Result(int neuron_id);
        void Add_Cor_Value(double current_value);

        // As these vectors are being pushed at different time, but they should be the same in size,
        // so to prevent any inconsistency, add a synchronize check function here
        int Synchronize_Check();

    private:

        std::vector<std::string> result_name;
        std::vector<std::vector<int>> raw_results;
        std::vector<std::vector<double>> cor_value;
        std::vector<int> target;
};

class LWSVR {

    public:

        LWSVR(struct LAMSTARNNCONF lamconf);
        LWSVR(std::ifstream *read_file);
        ~LWSVR();

        double Get_SOM_Link_Weights(int module1, int module2, int idx_in_module1, int idx_in_module2);
        double Get_Outlayer_Link_Weights(int module_in, int module_out, int idx_in_module_in, int idx_in_module_out);

        void Reinforce_SOM_Link_Weights(int module1, int module2, int idx_in_module1, int idx_in_module2);

        void Reinforce_Outlayer_Link_Weights(int module_in, int module_out, int idx_in_module_in, int idx_in_module_out, double factor);

        void Punish_Outlayer_Link_Weights(int module_in, int module_out, int idx_in_module_in, int idx_in_module_out, double factor);

        void Save_Link_Weights(std::ofstream *save_file);
        
        double linear_inner_lw_reinforcer;

        double linear_output_lw_reinforcer;

        double linear_output_lw_punishment;

        int bias_sensitive_enabled;

        double bias_sensitivity_value;

        double base_value_bs;

    protected:

    private:        
    
        // Configuration data
        int num_of_SOM_module;
        int SOM_resolution;
        int num_of_output_module;
        int output_module_resolution;

        /* Special things about SOM link weights data structure
            let's explain this as an example
            assume we want to access link weights between module 7 index 10 and module 9 index 3
            SOM link weights are inner link weights, so this is equivalent to module 9 index 3 and module 7 index 10,
            This two link weights are the same and should be stored in the same address, but accessing information is different
            so everytime we will put smaller module id first and calculate an offset to the larger module id
            In this case, we are actually accessing som_link_weights[7][9-7-1][10][3]

            offset should be larger - smaller - 1 as index starts from 0.
        */

        // som_link_weights[lower indexed module][higher indexed module]
        // [which neuron in lower indexed module][which neuron in higher indexed module]
        double ****som_link_weights;

        // outlayer_link_weights[which input module][which output module]
        // [which neuron in input module][which neuron in output module]
        double ****outlayer_link_weights;

};

struct LAMSTARNNCONF {
    int num_of_SOM_module;
    int *subword_dimension;
    int SOM_resolution;
    int num_of_output_module;
    int output_module_resolution;
};

struct SOMRESULT {

    // Determined by SOM neuron weights with input
    int primary_winning_neuron;

    // Determined by link weights
    int secondary_winning_neuron;

    // Determined by the difference between link weights strength
    double primary_proportion;

    // should add to one if primary winning neuron exists
    double secondary_proportion;
};

class LAMSTARNN {

    public:

        LAMSTARNN(struct LAMSTARNNCONF lamconf);
        LAMSTARNN(std::string read_name);
        ~LAMSTARNN();

        int Feed_SOM(int som_module_id, int subword_dimension, double *subword, int adjusted);
        void Batch_Feed_SOM_With_Stride(int stride, int subwords_length, int start_module, int end_module, double *subwords, int adjusted);

        /*
        LAMSTAR File Format:
        num_of_SOM_module SOM_resolution num_of_output_module output_module_resolution \n
        (SOMDATA) subword_dimension SOM_resolution learning rate
        weight by neuron connect to input
        ...
        (LinkWeights DATA) num_of_SOM_module SOM_resolution num_of_output_module output_module_resolution \n
        blahblah, please read source code LOL
        */
        void Save_LAMSTARNN(std::string save_name);

        void Stdout_Raw_Results();

        void Train_One_Output_Module(int output_module_id, int correct_neuron);

        void Batch_Train(int *correct_neurons, int start_module, int end_module);

        void Show_Status();

        void Enable_Biased_Sensitivity();

        void Disable_Biased_Sensitivity();

        void Set_Biased_Sensitivity_Value(double val);

        void Set_Linear_Inner_Reinforcer_Value(double val);

        void Set_Linear_Output_Reinforcer_Value(double val);

        void Set_Linear_Output_Punishment_Value(double val);

        void Set_Base_Value_Bs(double val);

        void Enable_Grouping_Punishment_Void();

        void Disable_Grouping_Punishment_Void();

        void Generate_Result(int force_proceed, std::string result_name);

        void Create_Report(std::string file_name);

        // Configuration data
        int num_of_SOM_module;
        int SOM_resolution;
        int num_of_output_module;
        int output_module_resolution;

        int enable_multithread;
        int maximum_threads;
    
    protected:

        void Record_SOM_Results_And_Weights_Postprocessing();
        
        void Determine_Winner_Modulewise(int output_module_id);

        void Batch_Determine_Winner();

        int Need_Punishment(int target_neuron, int winning_neuron);

    private:

        // Link Weight Server
        LWSVR *link_weights;

        // SOM modules
        SOMLAYER **som_modules;

        // SOM winner by module
        struct SOMRESULT **som_res;

        // Feeded module
        int *feeded_module;

        // output results by module
        int *winning_neuron_by_output_module;

        // maximum value a secondary neuron can change the result
        double flexibility;

        // enabled to stop punish if target neuron is odd or even as current output neuron
        int grouping_punishment_void_enabled;

        RESULT_COLLECTOR *res_collector;
};