#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <stack>
#include <cassert>
#include <map>
#include <memory>

#include "vm_symbol.hpp"

namespace popo {
    namespace stack_vm {

        class vm
        {
        public:
            vm (std::string ir_code)
                : ir_code_ss(ir_code) {}

        private:
            std::stringstream ir_code_ss;
            std::list< std::unique_ptr<instruction> > instruction_list;

            std::stack< std::unique_ptr<element> > stack;

            std::map<std::string, function> function_table;
            //std::stack<function> function_stack;
            //std::list<var> var_table;

            std::map< std::string, std::unique_ptr<symbol_entry> > symbol_table;

        public:
            auto parse() -> void
            {
                std::string s;
                std::list<std::string> ir_list;
                std::vector<std::string> inst_vec;

                while( std::getline(ir_code_ss, s, '\n') ) {
                    ir_list.push_back(s);
                }

                for (auto it = ir_list.begin(); it != ir_list.end(); ++it) {
                    std::stringstream inst_ss(*it);

                    while( std::getline(inst_ss, s, ' ') ){
                        inst_vec.push_back(s);
                    }

                    instruction_list.push_back( create_inst(inst_vec) );
                    inst_vec.clear();
                }

                run();
            }


        public:
            auto run(void) -> void
            {
                for(auto it = instruction_list.begin(); it != instruction_list.end(); ++it ) {

                    switch((*it)->op) {
                    case operation::main:
                        {
                            std::cout << "main function" << std::endl;
                            break;
                        }

                        // case operate::func:
                    //     {
                    //         std::cout << "function define" << std::endl;

                    //         const std::string func_name = it->name;
                    //         std::list<instruction> func_code;
                    //         int arg_num = 0;

                    //         while(it->op != operate::ret) {

                    //             if(it->op == operate::param) {
                    //                 arg_num++;
                    //             }

                    //             func_code.push_back(*it);
                    //             ++it;
                    //         }

                    //         //function_table.push_back(function func(func_name, arg_num, func_code););
                    //         function_table.insert( make_pair(func_name, function(func_name, arg_num, func_code) ) );

                    //         std::cout << func_name << std::endl;
                    //         //it = parse_func(it);
                    //         break;
                    //     }
                    default:
                        {
                            stack_manager( std::move(*it) );
                        }

                    }
                }
            }

        // public :
        //     auto parse_func(std::list<instruction>::iterator it) -> std::list<instruction>::iterator
        //     {


        //         return it;
        //     }


        public :
            auto stack_manager(std::unique_ptr<instruction> ins ) -> void
            {

                // std::stack<element> arg_stack;

                 switch(ins->op) {
                 case operation::ret :
                     {
                         break;
                     }
                 case operation::pop :
                     {
                         //arg_stack.push(stack.top());
                         //stack.pop();
                         break;
                     }
                 case operation::write :
                     {
                         if (!stack.empty()) {
                             auto e = std::move(stack.top());

                             auto e_ = std::unique_ptr<int_element>
                                     (static_cast<int_element *>(e.release()));

                             std::cout << e_->data << std::endl;

                         } else {
                             std::cout << "Ooops, stack is empty :(" << std::endl;
                         }
                     }
                 case operation::push_int :
                // case operation::push_float :
                // case operation::push_string :
                // case operation::push_bool :
                // case operation::push_list :
                     {
                         auto op_ins = std::unique_ptr<op_instruction>
                             (static_cast<op_instruction *>(ins.release()));

                         stack.push(std::move(op_ins->operand));
                         break;
                     }
                 case operation::push_symbol :
                     {
                         auto op_ins = std::unique_ptr<op_instruction>
                             (static_cast<op_instruction *>(ins.release()));

                         // auto sym_it = symbol_table.find(op_ins->operand);

                         // if (sym_it != symbol_table.end() ) {

                         //     if ((*sym_it)->sclass == sym_class::var) {

                         //         stack.push( element(sym_it->name, it->data) );
                         //     }

                         // }
                         // else {

                         stack.push(std::move(op_ins->operand));

                         //}

                         break;
                     }

                 case operation::apply :
                     {
                         auto func_e = std::move( stack.top() );
                         stack.pop();

                         switch(func_e->type) {
                         case element_type::add :
                             {
                                 auto e1 = std::move( stack.top() );
                                 stack.pop();

                                 auto e1_ = std::unique_ptr<int_element>
                                     (static_cast<int_element *>(e1.release()));

                                 // switch(e1->type) {
                                 // case element_type::integer :

                                 //     auto e1_ = std::unique_ptr<int_element>
                                 //         (static_cast<int_element *>(e1.release()));
                                 //     break;


                                 // }

                                 auto e2 = std::move( stack.top() );
                                 stack.pop();

                                 auto e2_ = std::unique_ptr<int_element>
                                     (static_cast<int_element *>(e2.release()));


                                 // switch(e2->type) {
                                 // case element_type::integer :
                                 //     auto e2_ = std::unique_ptr<int_element>
                                 //         (static_cast<int_element *>(e2.release()));
                                 //     break;
                                 // }

                                 std::unique_ptr<element> e3( new int_element( e1_->data + e2_->data));


                                 // auto po = std::unique_ptr<element>(e2);

                                 // std::cout << "abbbabba" << std::endl;
                                 stack.push(std::move(e3));

                                 //auto add = [](int a, int b){ return a + b; };
                                 //calc(stack, add);
                                 break;
                             }
                //         case element_type::sub :
                //             {
                //                 auto sub = [](int a, int b){ return b - a; };
                //                 calc(stack, sub);
                //                 break;
                //             }
                //         case element_type::mul :
                //             {
                //                 auto mul = [](int a, int b){ return a * b; };
                //                 calc(stack, mul);
                //                 break;
                //             }
                //         case element_type::data :
                //             {
                //                 // for(auto it = function_table.begin(); it != function_table.end(); ++it)
                //                 //     {
                //                 //         if (it->name == e_.operand) {
                //                 //             std::cout << "function " << e_.operand << " is called " << std::endl;
                //                 //             //run_function(*it);
                //                 //         }
                //                 //     }
                //                 break;
                //             }
                //         case element_type::define :
                //             {
                //                 element name_e = stack.top();
                //                 stack.pop();
                //                 element data_e = stack.top();
                //                 stack.pop();

                //                 auto v = std::unique_ptr<var_entry>( new var_entry(name_e.operand, data_e.type, data_e.operand) );
                //                 //symbol_table.insert( make_pair(name_e.operand, v) );

                //                 std::cout << "define var " << v->name << "  "<< v->data << std::endl;

                //                 // for(auto it = inst_list.begin();
                //                 //     it != inst_list.end(); ++it) {
                //                 //     if (it->elm.operand == v.label ) {
                //                 //         std::cout << "aa" << std::endl;
                //                 //         it->op =
                //                 //         it->elm = element(data_e.type, v.data);
                //                 //     }
                //                 // }

                             break;
                         }


                //         default :
                //             {}
                //         }
                //         break;
                     }

                     //     assert(!arg_stack.empty());
                 }
            }


            // auto run_function(function func) -> void
            // {
            //     for(auto inst_it = func.code.begin();
            //         inst_it != func.code.end(); ++inst_it) {

            //         if (inst_it->op == operation::param) {

            //             var v(inst_it->name, stack.top().operand, stack.top().type);
            //             stack.pop();
            //             func.var_table.push_back(v);

            //             for(auto it = func.code.begin();
            //                 it != func.code.end(); ++it) {

            //                 if (it->elm.operand == v.label ) {
            //                     it->elm = element(element_type::data, v.data);
            //                 }
            //             }


            //         } else {
            //             stack_manager(*inst_it);
            //         }
            //     }
            // }


            // template<typename T, typename Fanc>
            // auto calc(std::stack<T>, Fanc f) -> void
            // {
            //     element e1 = stack.top();
            //     stack.pop();
            //     element e2 = stack.top();
            //     stack.pop();

            //     auto num1 = std::stoi(e1.operand);
            //     auto num2 = std::stoi(e2.operand);

            //     element e3(element_type::data, std::to_string( f(num1,num2) ) );
            //     stack.push(e3);
            // }



        public:
            auto create_inst(std::vector<std::string> inst_vec) -> std::unique_ptr<instruction>
            {
                const std::string op_s = inst_vec[0];
                operation op;

                if (op_s == "\tpop") {
                    op = operation::pop;
                }
                else if (op_s == "\tapply") {
                    op = operation::apply;
                }
                else if (op_s == "\treturn") {
                    op = operation::ret;
                }
                else if (op_s == "\tread") {
                    op = operation::read;
                }
                else if (op_s == "\twrite") {
                    op = operation::write;
                }
                else if (op_s == "main:") {
                    op = operation::main;
                }

                // else if (*(op_s.end() - 1 ) == ':') {
                //     std::string fn_name = op_s;
                //     fn_name.erase(fn_name.end() - 1);
                //     return instruction(operation::func, fn_name);
                // }

                else if (op_s == "\tpush_int") {
                    //std::unique_ptr<element> int_e(new int_element( std::stoi(inst_vec[1])) );
                    //return std::unique_ptr<op_instruction>( new op_instruction(operation::push_int, int_e) );
                    return std::unique_ptr<op_instruction>
                        ( new op_instruction( operation::push_int, std::unique_ptr<element>
                                             ( new int_element( std::stoi( inst_vec[1] ) ) ) ) );
                }

                // else if (op_s == "\tpush_float") {
                //     return instruction(operation::push_float, element(element_type::real, inst_vec[1]));
                // }
                // else if (op_s == "\tpush_string") {
                //     return instruction(operation::push_string, element(element_type::string, inst_vec[1]));
                // }
                // else if (op_s == "\tpush_bool") {
                //     return instruction(operation::push_bool, element(element_type::boolean, inst_vec[1]));
                // }
                // else if (op_s == "\tpush_list") {
                //     return instruction(operation::push_list, element(element_type::list, inst_vec[1]));
                // }

                else if (op_s == "\tpush_symbol") {
                    return std::unique_ptr<op_instruction>
                        ( new op_instruction( operation::push_symbol, create_op_element( inst_vec[1] ) ) );
                }
                // else if (op_s == "\tparam") {
                //     op = operation::param;
                // }
                // else if (op_s == "\tmake_clojure") {
                //     op = operation::make_clojure;
                // }

                // // op_s == "branch"
                // else {
                //     op = operation::branch;
                // }

                return std::unique_ptr<instruction>( new instruction(op) );

            }

         public:
            auto create_op_element(const std::string operand) -> std::unique_ptr<element>
            {
                 element_type type;

                 if (operand == "+") {
                     type = element_type::add;
                 }
                 else if (operand == "-") {
                     type = element_type::sub;
                 }
                 else if (operand == "*") {
                     type = element_type::mul;
                 }
                 else if (operand == "/") {
                     type = element_type::div;
                 }
                 else if (operand == "=") {
                     type = element_type::eq;
                 }
                 else if (operand == "<") {
                     type = element_type::lt;
                 }
                 else if (operand == ">") {
                     type = element_type::mt;
                 }
                 else if (operand == "<=") {
                     type = element_type::lte;
                 }
                 else if (operand == ">=") {
                     type = element_type::lte;
                 }
                 else if (operand == "&") {
                     type = element_type::land;
                 }
                 else if (operand == "|") {
                     type = element_type::lor;
                 }
                 else if (operand == "define") {
                     type = element_type::define;
                 }
                 else {
                     return std::unique_ptr<element>( new func_element(operand) );
                 }

                 return std::unique_ptr<element>( new element(type) );
             }

        };

    }
}
