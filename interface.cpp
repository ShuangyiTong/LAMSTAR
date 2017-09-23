#include "interface.hpp"

using namespace hjbase::ufunc;
using namespace hjbase::cnt::common_msgs;
using namespace hjlam::cnt;
using namespace hjlam::msgs;
/*
    LAMSTAR controlling functions
*/
std::string hjlam::LAMINTERFACE::Creat_New_LAMSTARNN_Obj(const const_itVecStr& vals) {
    LAMSTARNNCONF lamconf;
    try {
        lamconf.num_of_SOM_module = std::stoi(vals.at(0));
        lamconf.SOM_resolution = std::stoi(vals.at(1));
        lamconf.num_of_output_module = std::stoi(vals.at(2));
        lamconf.output_module_resolution = std::stoi(vals.at(3));
    }
    catch (const std::logic_error& le) {
        Signal_Error(NE_CONVERSION_FAILED, vals);
        throw hjbase::except::eval_except;
    }

    int subword_dimension[lamconf.num_of_SOM_module];
    int cur = 0;
    if(vals.size()==4) {
        std::cout<<IA_ENTER_DIM<<std::endl;
        while (cur<lamconf.num_of_SOM_module) {
            int times;
            int dim;
            std::cin>>times>>dim;
            for(int i=0;i<times;i++) {
                subword_dimension[cur+i] = dim;
            }
            cur+=times;
        }
    }
    else {
        int reading_position = 3;
        while (reading_position<vals.size()&&cur<lamconf.num_of_SOM_module) {
            int times;
            int dim;
            try {
                times = std::stoi(vals.at(++reading_position));
                dim = std::stoi(vals.at(++reading_position));
            }
            catch (const std::logic_error& le) {
                Signal_Error(NE_CONVERSION_FAILED, vals);
                throw hjbase::except::eval_except;
            }
            for(int i=0;i<times;i++) {
                subword_dimension[cur+i] = dim;
            }
            cur+=times;
        }
    }

    if(cur!=lamconf.num_of_SOM_module) {
        std::cout<<LAM_WA_INCO_DIM<<std::endl;
    }

    lamconf.subword_dimension = subword_dimension;
    
    LAMSTARNN* lam_ptr = new LAMSTARNN(lamconf);

    std::string lam_ptrStr = LAMSTAR_TAG + Convert_Ptr_To_Str(lam_ptr);

    lamstar_pool->insert(std::pair<std::string, LAMSTARNN*>(lam_ptrStr, lam_ptr));

    return lam_ptrStr;
}

hjlam::LAMINTERFACE::LAMINTERFACE()
    : hjbase::HUAJISCRIPTBASE() {
    lamstar_pool = new std::map<std::string, LAMSTARNN*>;
}

hjlam::LAMINTERFACE::~LAMINTERFACE() {
    for(std::map<std::string, LAMSTARNN*>::iterator it=lamstar_pool->begin();it!=lamstar_pool->end();++it) {
        delete it->second;
    }
    delete lamstar_pool;
}

std::pair<int, bool> hjlam::LAMINTERFACE::More_On_Command_Level_1(const const_itVecStr& command) {
    return More_On_Command_Level_2(command);
}

std::pair<int, bool> hjlam::LAMINTERFACE::More_On_Command_Level_2(const const_itVecStr& command) {
    return std::pair<int, bool>(-1, false);
}

std::pair<int, bool> hjlam::LAMINTERFACE::More_On_Builtin_Function_Search_Level_1(const std::string& op) {
    std::map<std::string, int>::const_iterator op_key_it = BUILTIN_FUNCTIONS.find(op);
    if(op_key_it!=BUILTIN_FUNCTIONS.end()) {
        // Inform hjbase::HUAJISCRIPTBASE::EVALUATE_EXPRESSION I have my built-in function
        return std::pair<int, bool>(op_key_it->second, true);
    }
    else {
        return More_On_Builtin_Function_Search_Level_2(op);
    }
}

std::pair<int, bool> hjlam::LAMINTERFACE::More_On_Builtin_Function_Search_Level_2(const std::string& op) {
    return std::pair<int, bool>(-1, false);
}

std::string hjlam::LAMINTERFACE::More_On_Builtin_Function_Level_1(const std::string& op, int op_key, const const_itVecStr& vals) {
    switch (op_key) {
        case eFUNC_LAMSTAR_NEW: {
            return Creat_New_LAMSTARNN_Obj(vals);
        }
        default:
            return More_On_Builtin_Function_Level_2(op, op_key, vals);
    }
}

std::string hjlam::LAMINTERFACE::More_On_Builtin_Function_Level_2(const std::string& op, int op_key, const const_itVecStr& vals) {
    throw hjbase::except::huaji_except;
}

std::pair<std::string, bool> hjlam::LAMINTERFACE::More_On_Names_Query_Level_1(const std::string& name) {
    return More_On_Names_Query_Level_2(name);
}

std::pair<std::string, bool> hjlam::LAMINTERFACE::More_On_Names_Query_Level_2(const std::string& name) {
    return std::pair<std::string, bool>(std::string(), false);
}

std::pair<std::string, bool> hjlam::LAMINTERFACE::More_On_Slice_Operator_Level_1(const const_itVecStr& vals) {
    return More_On_Slice_Operator_Level_2(vals);
}

std::pair<std::string, bool> hjlam::LAMINTERFACE::More_On_Slice_Operator_Level_2(const const_itVecStr& vals) {
    return std::pair<std::string, bool>(std::string(), false);
}

void hjlam::LAMINTERFACE::More_Cleanup_Level_1() {
    return More_Cleanup_Level_2();
}

void hjlam::LAMINTERFACE::More_Cleanup_Level_2() {
    return;
}

void hjlam::LAMINTERFACE::More_On_Config_Level_1(const std::string& item, const std::string& status) {
    return More_On_Config_Level_2(item, status);
}

void hjlam::LAMINTERFACE::More_On_Config_Level_2(const std::string& item, const std::string& status) {
    Signal_Error(IE_UNDEFINED_NAME, item);
    return;
}

void hjlam::LAMINTERFACE::More_On_Info_Level_1(const const_itVecStr& command) {
    return More_On_Info_Level_2(command);
}

void hjlam::LAMINTERFACE::More_On_Info_Level_2(const const_itVecStr& command) {
    Signal_Error(IE_UNDEFINED_NAME, command.at(1));
    return;
}

std::pair<bool, int> hjlam::LAMINTERFACE::More_On_Check_If_GC_Required_Level_1(const std::string& ref_val) {
    // No need to use RBt
    if(Starts_With(ref_val, LAMSTAR_TAG)) {
        return std::pair<bool, int>(true, LAMSTAR_TAG_CODE);
    }
    return More_On_Check_If_GC_Required_Level_2(ref_val);
}

std::pair<bool, int> hjlam::LAMINTERFACE::More_On_Check_If_GC_Required_Level_2(const std::string& ref_val) {
    return std::pair<bool, int>(false, -1);
}

void hjlam::LAMINTERFACE::More_On_Delete_Object_Level_1(const std::string& ref_val, int type_code) {
    if(type_code==LAMSTAR_TAG_CODE) {
        std::map<std::string, LAMSTARNN*>::const_iterator lamstar_it
            = lamstar_pool->find(ref_val);

        if(lamstar_it==lamstar_pool->end()) {
            #ifdef SIGNAL_UNABLE_TO_DELETE
                Signal_Error(IE_NOT_DELETABLE, ref_val);
            #endif
            return;
        }

        delete lamstar_it->second;
        lamstar_pool->erase(lamstar_it);

        return;
    }
    else {
        return More_On_Delete_Object_Level_2(ref_val, type_code);
    }
}

void hjlam::LAMINTERFACE::More_On_Delete_Object_Level_2(const std::string& ref_val, int type_code) {
    Signal_Error(IE_NOT_DELETABLE, ref_val);
    return;
}