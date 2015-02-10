#pragma once
#include <string>
#include <vector>
#include <list>
#include <cassert>
#include <memory>

// #include "semantic.hpp"

namespace popo {
    namespace ir {

        class ir_manager {
        public:
            ir_manager()
                : ir_define_(),
                  ir_procedure_(),
                  closure_number_(0),
                  is_define(false),
                  symbol() {};

        private:
            std::list<std::string> ir_define_;
            std::list<std::string> ir_procedure_;

            int closure_number_;
            bool is_define;
            std::string symbol;

        public:
            auto add_procedure(std::list<std::string> list)
                -> void
            {
                ir_procedure_.insert(ir_procedure_.end(), list.begin(), list.end());
            }
//             auto create_clojure(std::string name = "") -> std::string
//             {
//                 std::string label("");
//                     label += "clojure_";
//                     label += std::to_string(closure_number_++);
//                 ir_define_.push_back(label + ":");
//                 return label;
//             }

//             auto push_params(std::string p) -> void
//             {
//                 std::string param = "\tparam " + p;
//                 ir_define_.push_back(param);
//             }

//             auto push_args(std::string p) -> void
//             {
//                 std::string param = "\tpush " + p;
//                 if (is_define) {
//                     ir_define_.push_back(param);
//                 } else {
//                     ir_procedure_.push_back(param);
//                 }
//             }

//             auto create_label(std::string l) -> void
//             {
//                 ir_define_.push_back(l + ":");
//             }

//             auto create_function(std::string f) -> void
//             {
//                 std::string symbol = "\tpush " + f;
//                 std::string apply = "\tapply";
//                 if (is_define) {
//                     ir_define_.push_back(symbol);
//                     ir_define_.push_back(apply);
//                 } else {
//                     ir_procedure_.push_back(symbol);
//                     ir_procedure_.push_back(apply);
//                 }
//             }

//             auto define_enable() -> void
//             {
//                 is_define = true;
//             }

//             auto define_disable() -> void { is_define = false; }

            auto output_ir_code(void) -> void
            {
                for (auto&& def_st : ir_define_) {
                    std::cout << def_st << std::endl;
                }

                std::cout << "main:" << std::endl;
                for (auto&& pro_st : ir_procedure_) {
                    std::cout << pro_st << std::endl;
                }
            }
        };
    }
} // namespace popo
