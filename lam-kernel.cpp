#include "lam-kernel.hpp"

RESULT_COLLECTOR::RESULT_COLLECTOR() {}
RESULT_COLLECTOR::~RESULT_COLLECTOR() {}

int RESULT_COLLECTOR::Synchronize_Check() {
   if(result_name.size()!=raw_results.size()||raw_results.size()!=cor_value.size()) {
       std::cout<<std::endl<<"Warning: Web report not synchronized"<<std::endl;
       return 0;
   }
   return 1;
}

void RESULT_COLLECTOR::Add_New_Result(std::string name) {
    Synchronize_Check();
    result_name.push_back(name);
    raw_results.push_back(std::vector<int>(0));
    cor_value.push_back(std::vector<double>(0));
    return;
}

void RESULT_COLLECTOR::Add_Raw_Result(int neuron_id) {
    raw_results[raw_results.size()-1].push_back(neuron_id);
    return;
}

void RESULT_COLLECTOR::Add_Cor_Value(double current_value) {
    cor_value[raw_results.size()-1].push_back(current_value);
    return;
}

LWSVR::LWSVR(struct LAMSTARNNCONF lamconf)
    : num_of_SOM_module (lamconf.num_of_SOM_module)
    , SOM_resolution (lamconf.SOM_resolution)
    , num_of_output_module (lamconf.num_of_output_module)
    , output_module_resolution (lamconf.output_module_resolution) 
    , linear_inner_lw_reinforcer (0.1)
    , linear_output_lw_reinforcer (0.1) 
    , linear_output_lw_punishment (0.1)
    , bias_sensitive_enabled (0) 
    , bias_sensitivity_value (1.0)
    , base_value_bs (0.5) {

    // set SOM link weights
    som_link_weights = new double***[num_of_SOM_module-1];

    // lower indexed module layer, no need for the highest indexed neuron
    // as it won't link it self
    for(int i=0;i<num_of_SOM_module-1;i++) {

        // There are num_of_SOM_module-i-1 higher indexed SOM modules
        som_link_weights[i] = new double**[num_of_SOM_module-i-1];
        for(int j=0;j<num_of_SOM_module-i-1;j++) {
            som_link_weights[i][j] = new double*[SOM_resolution];
            for(int k=0;k<SOM_resolution;k++) {
                som_link_weights[i][j][k] = new double[SOM_resolution]();

                // Initialize link weights to a very small positive value
                for(int l=0;l<SOM_resolution;l++) {
                    som_link_weights[i][j][k][l] = my_rand()/(RAND_MAX*10);
                }
            }
        }
    }

    // set outlayer link weights
    outlayer_link_weights = new double***[num_of_SOM_module];

    for(int i=0;i<num_of_SOM_module;i++) {
        outlayer_link_weights[i] = new double**[num_of_output_module];
        for(int j=0;j<num_of_output_module;j++) {
            outlayer_link_weights[i][j] = new double*[SOM_resolution];
            for(int k=0;k<SOM_resolution;k++) {
                outlayer_link_weights[i][j][k] = new double[output_module_resolution];

                // Initialize link weights to output to a small positive value
                for(int l=0;l<output_module_resolution;l++) {
                    outlayer_link_weights[i][j][k][l] = my_rand()/(RAND_MAX*3);
                }
            }
        }
    }
}

LWSVR::LWSVR(std::ifstream *read_file) {
    (*read_file)>>num_of_SOM_module;
    (*read_file)>>SOM_resolution;
    (*read_file)>>num_of_output_module;
    (*read_file)>>output_module_resolution;
    (*read_file)>>linear_inner_lw_reinforcer;
    (*read_file)>>linear_output_lw_reinforcer;
    (*read_file)>>linear_output_lw_punishment;
    (*read_file)>>bias_sensitive_enabled;
    (*read_file)>>bias_sensitivity_value;
    (*read_file)>>base_value_bs;
    som_link_weights = new double***[num_of_SOM_module-1];
    for(int i=0;i<num_of_SOM_module-1;i++) {
        som_link_weights[i] = new double**[num_of_SOM_module-i-1];
        for(int j=0;j<num_of_SOM_module-i-1;j++) {
            som_link_weights[i][j] = new double*[SOM_resolution];
            for(int k=0;k<SOM_resolution;k++) {
                som_link_weights[i][j][k] = new double[SOM_resolution]();
                for(int l=0;l<SOM_resolution;l++) {
                    (*read_file)>>som_link_weights[i][j][k][l];
                }
            }
        }
    }
    outlayer_link_weights = new double***[num_of_SOM_module];
    for(int i=0;i<num_of_SOM_module;i++) {
        outlayer_link_weights[i] = new double**[num_of_output_module];
        for(int j=0;j<num_of_output_module;j++) {
            outlayer_link_weights[i][j] = new double*[SOM_resolution];
            for(int k=0;k<SOM_resolution;k++) {
                outlayer_link_weights[i][j][k] = new double[output_module_resolution];
                for(int l=0;l<output_module_resolution;l++) {
                    (*read_file)>>outlayer_link_weights[i][j][k][l];
                }
            }
        }
    }
}

LWSVR::~LWSVR() {
    for(int i=0;i<num_of_SOM_module-1;i++) {
        for(int j=0;j<num_of_SOM_module-i-1;j++) {
            for(int k=0;k<SOM_resolution;k++) {
                delete [] som_link_weights[i][j][k];
            }
            delete [] som_link_weights[i][j];
        }        
        delete [] som_link_weights[i];
    }
    delete [] som_link_weights;

    for(int i=0;i<num_of_SOM_module;i++) {
        for(int j=0;j<num_of_output_module;j++) {
            for(int k=0;k<SOM_resolution;k++) {
                delete [] outlayer_link_weights[i][j][k];
            }
            delete [] outlayer_link_weights[i][j];
        }
        delete [] outlayer_link_weights[i];
    }
    delete [] outlayer_link_weights;
}

void LWSVR::Save_Link_Weights(std::ofstream *save_file) {
    (*save_file)<<num_of_SOM_module<<" ";
    (*save_file)<<SOM_resolution<<" ";
    (*save_file)<<num_of_output_module<<" ";
    (*save_file)<<output_module_resolution<<" ";
    (*save_file)<<linear_inner_lw_reinforcer<<" ";
    (*save_file)<<linear_output_lw_reinforcer<<" ";
    (*save_file)<<linear_output_lw_punishment<<" ";
    (*save_file)<<bias_sensitive_enabled<<" ";
    (*save_file)<<bias_sensitivity_value<<" ";
    (*save_file)<<base_value_bs<<std::endl;
    for(int i=0;i<num_of_SOM_module-1;i++) {
        for(int j=0;j<num_of_SOM_module-i-1;j++) {
            for(int k=0;k<SOM_resolution;k++) {
                for(int l=0;l<SOM_resolution;l++) {
                    (*save_file)<<som_link_weights[i][j][k][l]<<" ";
                }
                (*save_file)<<std::endl;
            }
        }
    }

    for(int i=0;i<num_of_SOM_module;i++) {
        for(int j=0;j<num_of_output_module;j++) {
            for(int k=0;k<SOM_resolution;k++) {
                for(int l=0;l<output_module_resolution;l++) {
                    (*save_file)<<outlayer_link_weights[i][j][k][l]<<" ";
                }
                (*save_file)<<std::endl;
            }
        }
    }
}

double LWSVR::Get_SOM_Link_Weights(int module1, int module2, int idx_in_module1, int idx_in_module2) {

    if(module1==module2) {
        throw -1;
    }

    if(module1>module2) {
        std::swap(module1,module2);
    }

    int offset = module2-module1-1;

    return som_link_weights[module1][offset][idx_in_module1][idx_in_module2];
}

void LWSVR::Reinforce_SOM_Link_Weights(int module1, int module2, int idx_in_module1, int idx_in_module2) {

    if(module1==module2) {
        throw -1;
    }

    if(module1>module2) {
        std::swap(module1,module2);
    }

    int offset = module2-module1-1;

    som_link_weights[module1][offset][idx_in_module1][idx_in_module2]+=linear_inner_lw_reinforcer;

    return;
}

double LWSVR::Get_Outlayer_Link_Weights(int module_in, int module_out, int idx_in_module_in, int idx_in_module_out) {
    return outlayer_link_weights[module_in][module_out][idx_in_module_in][idx_in_module_out];
}

void LWSVR::Reinforce_Outlayer_Link_Weights(int module_in, int module_out, int idx_in_module_in, int idx_in_module_out, double factor) {
    outlayer_link_weights[module_in][module_out][idx_in_module_in][idx_in_module_out]
    // Add base_value_bs to prevent strange things when module_out = 0
    +=linear_output_lw_reinforcer*factor*(bias_sensitive_enabled?pow(module_out+base_value_bs,bias_sensitivity_value):1.0);
    return;
}

void LWSVR::Punish_Outlayer_Link_Weights(int module_in, int module_out, int idx_in_module_in, int idx_in_module_out, double factor) {
    outlayer_link_weights[module_in][module_out][idx_in_module_in][idx_in_module_out]-=linear_output_lw_punishment*factor;
    return;
}

LAMSTARNN::LAMSTARNN(struct LAMSTARNNCONF lamconf) 
    : num_of_SOM_module (lamconf.num_of_SOM_module)
    , SOM_resolution (lamconf.SOM_resolution)
    , num_of_output_module (lamconf.num_of_output_module)
    , output_module_resolution (lamconf.output_module_resolution)
    , flexibility (0.5)
    , grouping_punishment_void_enabled (1)
    , enable_multithread (1) 
    , maximum_threads (8) {
    res_collector = new RESULT_COLLECTOR();
    link_weights = new LWSVR(lamconf);
    som_modules = new SOMLAYER*[num_of_SOM_module];
    som_res = new SOMRESULT*[num_of_SOM_module];
    for(int i=0;i<num_of_SOM_module;i++) {
        som_modules[i] = new SOMLAYER(lamconf.subword_dimension[i], SOM_resolution);
        som_res[i] = new SOMRESULT();
    }
    feeded_module = new int[num_of_SOM_module]();
    winning_neuron_by_output_module = new int[num_of_output_module]();
    std::cout<<std::endl<<std::endl<<"LAMSTAR Neural Network object created:"<<std::endl;
    std::cout<<"    Number of SOM modules: "<<num_of_SOM_module<<std::endl;
    std::cout<<"    SOM resolution: "<<SOM_resolution<<std::endl;
    std::cout<<"    Number of output modules: "<<num_of_output_module<<std::endl;
    std::cout<<"    Output module resolution: "<<output_module_resolution<<std::endl;
    std::cout<<"    Subword dimensions: ";
    for(int i=0;i<num_of_SOM_module;i++) {
        std::cout<<lamconf.subword_dimension[i]<<" ";
    }
    std::cout<<std::endl<<std::endl<<std::endl;
    return;
}

LAMSTARNN::LAMSTARNN(std::string read_name) {
    std::ifstream *read_file = new std::ifstream(read_name);
    (*read_file)>>num_of_SOM_module;
    (*read_file)>>SOM_resolution;
    (*read_file)>>num_of_output_module;
    (*read_file)>>output_module_resolution;
    (*read_file)>>flexibility;
    (*read_file)>>grouping_punishment_void_enabled;
    (*read_file)>>maximum_threads;
    enable_multithread = 1;
    res_collector = new RESULT_COLLECTOR();
    som_modules = new SOMLAYER*[num_of_SOM_module];
    som_res = new SOMRESULT*[num_of_SOM_module];
    for(int i=0;i<num_of_SOM_module;i++) {
        som_modules[i] = new SOMLAYER(read_file);
        som_res[i] = new SOMRESULT();
    }
    link_weights = new LWSVR(read_file);
    feeded_module = new int[num_of_SOM_module]();
    winning_neuron_by_output_module = new int[num_of_output_module]();
    delete read_file;
}

LAMSTARNN::~LAMSTARNN() {
    delete link_weights;
    for(int i=0;i<num_of_SOM_module;i++) {
        delete som_modules[i];
        delete som_res[i];
    }
    delete [] som_modules;
    delete [] som_res;
    delete [] feeded_module;
    delete [] winning_neuron_by_output_module;
}

void LAMSTARNN::Save_LAMSTARNN(std::string save_name) {
    std::ofstream *save_file = new std::ofstream(save_name);
    (*save_file)<<num_of_SOM_module<<" ";
    (*save_file)<<SOM_resolution<<" ";
    (*save_file)<<num_of_output_module<<" ";
    (*save_file)<<output_module_resolution<<" ";
    (*save_file)<<flexibility<<" ";
    (*save_file)<<grouping_punishment_void_enabled<<std::endl;
    (*save_file)<<maximum_threads;
    for(int i=0;i<num_of_SOM_module;i++) {
        som_modules[i]->Save_SOM(save_file);
    }
    link_weights->Save_Link_Weights(save_file);

    delete save_file;
}

int LAMSTARNN::Feed_SOM(int som_module_id, int subword_dimension, double *subword, int adjusted) {

    // Verify if input subword_dimension matches number of input values in corresponding som module
    if(subword_dimension!=som_modules[som_module_id]->number_of_input_values) {
        std::cout<<"Subword dimension mismatch! Feeding data failed. Data address: "<<subword<<std::endl;
        return -1;
    }

    double temp_data[subword_dimension];
    for(int i=0;i<subword_dimension;i++) {
        temp_data[i] = subword[i];
    }

    if(adjusted) {
        Averaging_Data(temp_data, subword_dimension);
    }

    Vector_Normalization(temp_data, subword_dimension);

    if(feeded_module[som_module_id]) {
        std::cout<<std::endl<<"SOM module "<<som_module_id<<" already feeded, override"<<std::endl<<std::endl;
    }

    std::cout<<"SOM module "<<som_module_id<<": "<<std::endl;

    som_modules[som_module_id]->Send_Input_To_SOMLAYER(temp_data);

    feeded_module[som_module_id] = 1;

    return som_modules[som_module_id]->current_winner;
}

void LAMSTARNN::Batch_Feed_SOM_With_Stride(int stride, int subwords_length, int start_module, int end_module, double *subwords, int adjusted) {

    int cur = 0;
    if(enable_multithread) {
        std::cout.setstate(std::ios_base::failbit);
    }
    for(int i=start_module;i<=end_module;i++) {

        int required_dimension = som_modules[i]->number_of_input_values;
        if(cur+required_dimension>subwords_length) {
            std::cout<<"Warning: Data is not enough"<<std::endl;
            return;
        }
            
        Feed_SOM(i, required_dimension, &(subwords[cur]), adjusted);

        cur+=stride;
    }
    std::cout.clear();

    if(cur+som_modules[end_module]->number_of_input_values-stride!=subwords_length) {
        std::cout<<"Warning: Feeding completed, yet extra data left"<<std::endl;
    }

    return;
}

void LAMSTARNN::Generate_Result(int force_proceed, std::string result_name) {

    int proceed = 1;
    for(int i=0;i<num_of_SOM_module;i++) {
        if(!feeded_module[i]) {
            std::cout<<"Warning: SOM module "<<i<<" is not feeded yet."<<std::endl;
            proceed = 0;
        }
    }

    if(!proceed) {
        if(force_proceed) {
            std::cout<<std::endl<<"force_proceed enabled, continue"<<std::endl;
        }
        else {
            std::cout<<"force_proceed disabled, abort"<<std::endl;
            return;
        }
    }

    res_collector->Add_New_Result(result_name);

    Record_SOM_Results_And_Weights_Postprocessing();

    Batch_Determine_Winner();

}

void LAMSTARNN::Record_SOM_Results_And_Weights_Postprocessing() {
    for(int i=0;i<num_of_SOM_module;i++) {
        som_res[i]->primary_winning_neuron = som_modules[i]->current_winner;
    }    
    
    // Calculate link weights with winning neuron
    double *primary_winner_strength = new double[num_of_SOM_module]();
    for(int i=0;i<num_of_SOM_module;i++) {

        // No need to calculate link weights for winning neuron since there isn't any
        if(som_res[i]->primary_winning_neuron!=-1) {            
            for(int j=0;j<num_of_SOM_module;j++) {

                // ignore same module
                if(i!=j) {

                    // No need to calculate weights connect to a module with no winning neuron
                    if(som_res[j]->primary_winning_neuron!=-1) {
                        primary_winner_strength[i]+=link_weights->Get_SOM_Link_Weights(i,j,som_res[i]->primary_winning_neuron,som_res[j]->primary_winning_neuron);
                        link_weights->Reinforce_SOM_Link_Weights(i,j,som_res[i]->primary_winning_neuron,som_res[j]->primary_winning_neuron);
                    }
                }
            }
        }
    }

    // Calculate link weights to get a second candidate
    double *secondary_winner_strength = new double[num_of_SOM_module]();
    int *secondary_winner = new int[num_of_SOM_module];
    for(int i=0;i<num_of_SOM_module;i++) {
        // Iterate over all neurons
        for(int j=0;j<SOM_resolution;j++) {
            if(j!=som_res[i]->primary_winning_neuron) {
                double current_strength = 0.0;
                for(int k=0;k<num_of_SOM_module;k++) {
                    if(i!=k) {
                        if(som_res[k]->primary_winning_neuron!=-1) {
                            current_strength+=link_weights->Get_SOM_Link_Weights(i,k,j,som_res[k]->primary_winning_neuron);
                        }
                    }
                }
                if(current_strength>secondary_winner_strength[i]) {
                    secondary_winner_strength[i] = current_strength;
                    secondary_winner[i] = j;
                }
            }
        }

        som_res[i]->secondary_winning_neuron = secondary_winner[i];

    }

    // Determine primary proportion
    for(int i=0;i<num_of_SOM_module;i++) {

        // no winning neuron in this module
        if(som_res[i]->primary_winning_neuron==-1) {
            som_res[i]->primary_proportion = 0.0;
            som_res[i]->secondary_proportion = flexibility;
        }
        else {
            // winnning neuron exists
            som_res[i]->primary_proportion = 1.0 - flexibility*(secondary_winner_strength[i]/(secondary_winner_strength[i]+primary_winner_strength[i]));
            som_res[i]->secondary_proportion = 1.0 - som_res[i]->primary_proportion;
        }
    }

    std::cout<<std::endl<<"Primary Proportion by SOM module"<<std::endl<<"  "; 

    for(int i=0;i<num_of_SOM_module;i++) {
        std::cout<<som_res[i]->primary_proportion<<" ";
    }
    std::cout<<std::endl;

    delete [] primary_winner_strength;
    delete [] secondary_winner_strength;
    delete [] secondary_winner;
}

void LAMSTARNN::Determine_Winner_Modulewise(int output_module_id) {
    // Calculation by each neurons
    double current_max_value = -DBL_MAX;
    int max_neuron = -1;
    std::cout<<std::endl<<"Output module "<<output_module_id<<":"<<std::endl;
    for(int i=0;i<output_module_resolution;i++) {
        double current_value = 0.0;
        // Sum of all link weights
        for(int j=0;j<num_of_SOM_module;j++) {
            if(som_res[j]->primary_winning_neuron!=-1) {
                current_value += som_res[j]->primary_proportion*
                                 link_weights->Get_Outlayer_Link_Weights(j,output_module_id,som_res[j]->primary_winning_neuron,i);
            }
            current_value += som_res[j]->secondary_proportion*
                             link_weights->Get_Outlayer_Link_Weights(j,output_module_id,som_res[j]->secondary_winning_neuron,i);
        }

        std::cout<<"    neuron "<<i<<"'s value: "<<current_value<<std::endl;
        res_collector->Add_Raw_Result(i);
        res_collector->Add_Cor_Value(current_value);

        if(current_value>current_max_value) {
            current_max_value = current_value;
            max_neuron = i;
        }
    }

    if(max_neuron==-1) {
        std::cout<<"No maximum neuron found! To prevent any invalid memory accessing, terminate."<<std::endl;
        exit(1);
    }

    std::cout<<std::endl;

    winning_neuron_by_output_module[output_module_id] = max_neuron;
    return;
}

void LAMSTARNN::Batch_Determine_Winner() {
    for(int i=0;i<num_of_output_module;i++) {
        Determine_Winner_Modulewise(i);
    }
    return;
}

void LAMSTARNN::Stdout_Raw_Results() {
    std::cout<<"Output Module Results: "<<std::endl<<"    ";
    Print_Vector(winning_neuron_by_output_module, num_of_output_module);
    std::cout<<std::endl;
}

int LAMSTARNN::Need_Punishment(int target_neuron, int winning_neuron) {
    
    if(winning_neuron==target_neuron) {
        return 0;
    }
    
    if(grouping_punishment_void_enabled) {
        // always punish 0 if 0 is not the target
        if(!winning_neuron) {
            return 1;
        }

        // if target is 0
        if(!target_neuron) {
            // don't punish if winning neuron < 3
            if(target_neuron<3) {
                return 0;
            }
            else {
                return 1;
            }
        }

        // don't punish if they are in the same direction
        if(winning_neuron%2==target_neuron%2) {
            return 0;
        }
        else {
            return 1;
        }
    }
    else {
        return 0;
    }
}

void LAMSTARNN::Train_One_Output_Module(int output_module_id, int correct_neuron) {
    // Reward correct
    for(int i=0;i<num_of_SOM_module;i++) {
        if(som_res[i]->primary_winning_neuron!=-1) {
            link_weights->Reinforce_Outlayer_Link_Weights(i,output_module_id,som_res[i]->primary_winning_neuron,
                                                          correct_neuron, som_res[i]->primary_proportion);
        }
        link_weights->Reinforce_Outlayer_Link_Weights(i,output_module_id,som_res[i]->secondary_winning_neuron,
                                                      correct_neuron, som_res[i]->secondary_proportion);
    }
    // Punish incorrect one

    if(Need_Punishment(correct_neuron,winning_neuron_by_output_module[output_module_id])) {
        for(int i=0;i<num_of_SOM_module;i++) {
            if(som_res[i]->primary_winning_neuron!=-1) {
                link_weights->Punish_Outlayer_Link_Weights(i,output_module_id,som_res[i]->primary_winning_neuron,
                                                        winning_neuron_by_output_module[output_module_id], som_res[i]->primary_proportion);
            }
            link_weights->Punish_Outlayer_Link_Weights(i,output_module_id,som_res[i]->secondary_winning_neuron,
                                                    winning_neuron_by_output_module[output_module_id], som_res[i]->secondary_proportion);
        }
    }
    return;
}

void LAMSTARNN::Batch_Train(int *correct_neurons, int start_module, int end_module) {
    if(correct_neurons==NULL) {
        std::cout<<"Training target not given, abort batch training"<<std::endl;
        return;
    }
    for(int i=start_module;i<=end_module;i++) {
        Train_One_Output_Module(i,correct_neurons[i]);
    }
    return;
}

void LAMSTARNN::Create_Report(std::string file_name) {
    std::cout<<"Not complete implemented"<<std::endl;
}

void LAMSTARNN::Enable_Biased_Sensitivity() {
    link_weights->bias_sensitive_enabled = 1;
    return;
}

void LAMSTARNN::Disable_Biased_Sensitivity() {
    link_weights->bias_sensitive_enabled = 0;
    return;
}

void LAMSTARNN::Set_Biased_Sensitivity_Value(double val) {
    link_weights->bias_sensitivity_value = val;
    return;
}

void LAMSTARNN::Set_Linear_Inner_Reinforcer_Value(double val) {
    link_weights->linear_inner_lw_reinforcer = val;
    return;
}

void LAMSTARNN::Set_Linear_Output_Reinforcer_Value(double val) {
    link_weights->linear_output_lw_reinforcer = val;
    return;
}

void LAMSTARNN::Set_Linear_Output_Punishment_Value(double val) {
    link_weights->linear_output_lw_punishment = val;
    return;
}

void LAMSTARNN::Set_Base_Value_Bs(double val) {
    link_weights->base_value_bs = val;
    return;
}

void LAMSTARNN::Enable_Grouping_Punishment_Void() {
    grouping_punishment_void_enabled = 1;
    return;
}

void LAMSTARNN::Disable_Grouping_Punishment_Void() {
    grouping_punishment_void_enabled = 0;
    return;
}

void LAMSTARNN::Show_Status() {
    std::cout<<std::endl<<"Current LAMSTAR network status:"<<std::endl;
    std::cout<<"    Number of SOM modules: "<<num_of_SOM_module<<std::endl;
    std::cout<<"    SOM resolution: "<<SOM_resolution<<std::endl;
    std::cout<<"    Number of output modules: "<<num_of_output_module<<std::endl;
    std::cout<<"    Output module resolution: "<<output_module_resolution<<std::endl;
    std::cout<<"    Grouping punishment void enabled: "<<(grouping_punishment_void_enabled?"enabled":"disabled")<<std::endl;
    std::cout<<"    Value of linear inner link weight reinforcer: "<<link_weights->linear_inner_lw_reinforcer<<std::endl;
    std::cout<<"    Value of linear output link weight reinforcer: "<<link_weights->linear_output_lw_reinforcer<<std::endl;
    std::cout<<"    Value of linear output link weight punishment: "<<link_weights->linear_output_lw_punishment<<std::endl;
    std::cout<<"    Biased sensitivity in training: "<<(link_weights->bias_sensitive_enabled?"enabled":"disabled")<<std::endl;
    std::cout<<"    Biased sensitivity value: "<<link_weights->bias_sensitivity_value<<std::endl;
    std::cout<<"    Biased sensitivity bias: "<<link_weights->base_value_bs<<std::endl;
    std::cout<<"    Multithreading Support: "<<(enable_multithread?"enabled":"disabled")<<std::endl;
    return;
}