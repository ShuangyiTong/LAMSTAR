#pragma once

#include "hjscript.hpp"
#include "lam-kernel.hpp"

namespace hjlam {
    namespace cnt {
        const std::string FUNC_LAMSTAR_NEW = "lamstar-new";
        
        enum {
            eFUNC_LAMSTAR_NEW,
        };

        const std::map<std::string, int> BUILTIN_FUNCTIONS = {
            { FUNC_LAMSTAR_NEW, eFUNC_LAMSTAR_NEW },
        };

        const std::string LAMSTAR_TAG = "#:LAMSTAR";
        // Make sure the code is not the same as types in HUAJISCRIPTBASE
        const int LAMSTAR_TAG_CODE = 100;
    }
    namespace msgs {
        const std::string IA_ENTER_DIM = "Interactive dimension input: Please enter an array of subword dimensions in pair format";
        const std::string LAM_WA_INCO_DIM = "LAMSTAR Warning: Warning: Incorrect subword dimension";
    }

    class LAMINTERFACE : public hjbase::HUAJISCRIPTBASE {
        public:        
            LAMINTERFACE();
            ~LAMINTERFACE();

        protected:
            virtual std::pair<int, bool> More_On_Command_Level_2(const const_itVecStr& command);
            
            virtual std::pair<int, bool> More_On_Builtin_Function_Search_Level_2(const std::string& op);

            virtual std::string More_On_Builtin_Function_Level_2(const std::string& op, int op_key, const const_itVecStr& vals);

            virtual std::pair<std::string, bool> More_On_Names_Query_Level_2(const std::string& name);

            virtual std::pair<std::string, bool> More_On_Slice_Operator_Level_2(const const_itVecStr& vals);

            virtual void More_Cleanup_Level_2();

            virtual void More_On_Config_Level_2(const std::string& item, const std::string& status);

            virtual void More_On_Info_Level_2(const const_itVecStr& command);

            virtual std::pair<bool, int> More_On_Check_If_GC_Required_Level_2(const std::string&  ref_val);

            virtual void More_On_Delete_Object_Level_2(const std::string& ref_val, int type_code);

        private:

            std::pair<int, bool> More_On_Command_Level_1(const const_itVecStr& command) final;
            
            std::pair<int, bool> More_On_Builtin_Function_Search_Level_1(const std::string& op) final;

            std::string More_On_Builtin_Function_Level_1(const std::string& op, int op_key, const const_itVecStr& vals) final;

            std::pair<std::string, bool> More_On_Names_Query_Level_1(const std::string& name) final;

            std::pair<std::string, bool> More_On_Slice_Operator_Level_1(const const_itVecStr& vals) final;

            void More_Cleanup_Level_1() final;

            void More_On_Config_Level_1(const std::string& item, const std::string& status) final;

            void More_On_Info_Level_1(const const_itVecStr& command) final;

            std::pair<bool, int> More_On_Check_If_GC_Required_Level_1(const std::string&  ref_val) final;

            void More_On_Delete_Object_Level_1(const std::string& ref_val, int type_code) final;

            std::string Creat_New_LAMSTARNN_Obj(const const_itVecStr& vals);

            std::map<std::string, LAMSTARNN*>* lamstar_pool;
    };
}