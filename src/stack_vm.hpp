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
            ~vm() {}

        private:
            std::stringstream ir_code_ss;
            std::list<std::shared_ptr<instruction>> instruction_list;

            std::stack<std::shared_ptr<element>> stack;

            std::map<std::string, std::shared_ptr<function>> function_table;
            //std::stack<function> function_stack;
            //std::list<var> var_table;

            std::map< std::string, std::shared_ptr<symbol_entry>> symbol_table;

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
            auto run() -> void
            {
                for(auto it = instruction_list.begin(); it != instruction_list.end(); ++it ) {

                    switch((*it)->op) {
                    case operation::main:
                        {
                            std::cout << "main function" << std::endl;
                            break;
                        }

                    case operation::func:
                        {
                            std::cout << "function define" << std::endl;

                            auto op_ins = std::static_pointer_cast<op_instruction>(*it);
                            auto func_ele = std::static_pointer_cast<symbol_element>(op_ins->operand);

                            const std::string func_name = func_ele->data;

                            std::cout << func_name << std::endl;

                            std::list<std::shared_ptr<instruction>> func_code;
                            int arg_num = 0;
                            int ret_num = 1;

                            while(ret_num == 0) {

                                if((*it)->op == operation::ret) {
                                    ret_num--;
                                }
                                else if((*it)->op == operation::func) {
                                    ret_num++;
                                }

                                if((*it)->op == operation::param) {
                                    arg_num++;
                                }

                                func_code.push_back(std::move(*it));
                                ++it;
                            }


                            function_table.insert( make_pair(func_name, std::shared_ptr<function>
                                                             (new function(func_name, arg_num, std::move(func_code) ) ) ) );

                            std::cout << func_name << std::endl;
                            break;
                        }

                    default:
                        {
                            stack_manager( std::move(*it) );
                        }

                    }
                }
            }


        public :
            auto stack_manager(std::shared_ptr<instruction> ins ) -> void
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
                            stack.pop();

                            switch(e->type) {
                            case element_type::integer :
                                {
                                    auto e_int = std::static_pointer_cast<int_element>(e);

                                    std::cout << e_int->data << std::endl;
                                    break;
                                }
                            case element_type::real :
                                {
                                    auto e_real = std::static_pointer_cast<real_element>(e);
                                    std::cout << e_real->data << std::endl;
                                    break;
                                }
                            default:
                                std::cout << "not implemented" << std::endl;
                            }


                        } else {
                            std::cout << "Ooops, stack is empty :(" << std::endl;
                        }
                        break;
                    }

                case operation::push_int :
                case operation::push_float :
                    // case operation::push_string :
                    // case operation::push_bool :
                    // case operation::push_list :
                    {

                        auto op_ins = std::static_pointer_cast<op_instruction>(ins);

                        stack.push(std::move(op_ins->operand));
                        break;
                    }
                case operation::push_symbol :
                    {

                        auto op_ins = std::static_pointer_cast<op_instruction>(ins);

                        auto el = std::static_pointer_cast<symbol_element>(op_ins->operand);
                        assert(nullptr == el.get());
                        auto sym_it = symbol_table.find(el->data);

                        if (sym_it != symbol_table.end() ) {

                            if (sym_it->second->sclass == sym_class::var) {
                                auto e = std::static_pointer_cast<var_entry>( sym_it->second );
                                stack.push( e->data );
                            }

                        }
                        else {
                            op_ins->operand=std::move(el);
                            stack.push(std::move(op_ins->operand));
                        }

                        break;
                    }

                case operation::apply :
                    {
                        auto func_e = std::move( stack.top() );
                        stack.pop();
                        std::cout << "aababa" << std::endl;

                        switch(func_e->type) {
                        case element_type::add :
                            {

                                auto add_ii = [](int a, int b)->int{ return a + b; };
                                auto add_fi = [](float a, int b)->float{ return a + b; };
                                auto add_if = [](int a, float b)->float{ return a + b; };
                                auto add_ff = [](float a, float b)->float{ return a + b; };
                                calc(add_ii, add_fi, add_if, add_ff);
                                break;
                            }
                        case element_type::sub :
                            {
                                auto sub_ii = [](int a, int b)->int{ return a - b; };
                                auto sub_fi = [](float a, int b)->float{ return a - b; };
                                auto sub_if = [](int a, float b)->float{ return a - b; };
                                auto sub_ff = [](float a, float b)->float{ return a - b; };
                                calc(sub_ii, sub_fi, sub_if, sub_ff);
                                break;
                            }
                        case element_type::mul :
                            {
                                auto mul_ii = [](int a, int b)->int{ return a * b; };
                                auto mul_fi = [](float a, int b)->float{ return a * b; };
                                auto mul_if = [](int a, float b)->float{ return a * b; };
                                auto mul_ff = [](float a, float b)->float{ return a * b; };
                                calc(mul_ii, mul_fi, mul_if, mul_ff);
                                break;
                            }
                        case element_type::div :
                            {
                                auto div_ii = [](int a, int b)->int{ return a / b; };
                                auto div_fi = [](float a, int b)->float{ return a / b; };
                                auto div_if = [](int a, float b)->float{ return a / b; };
                                auto div_ff = [](float a, float b)->float{ return a / b; };
                                calc(div_ii, div_fi, div_if, div_ff);
                                break;
                            }

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
                        case element_type::define :
                            {
                                auto name_e = std::move(stack.top());
                                stack.pop();

                                auto data_e = std::move(stack.top());
                                stack.pop();

                                auto name_sym_e = std::static_pointer_cast<symbol_element>(name_e);

                                switch(data_e->type) {

                                case element_type::integer :
                                        {

                                            auto e_int = std::static_pointer_cast<int_element>(data_e);

                                            std::shared_ptr<symbol_entry> int_var
                                                ( new var_entry(name_sym_e->data, std::move(e_int)));

                                            symbol_table.insert(make_pair(name_sym_e->data, std::move(int_var)));

                                            break;
                                        }
                                case element_type::real:
                                    {
                                        auto e_real = std::static_pointer_cast<real_element>(data_e);

                                        std::shared_ptr<symbol_entry> real_var
                                            ( new var_entry(name_sym_e->data, std::move(e_real)));

                                        symbol_table.insert(make_pair(name_sym_e->data, std::move(real_var)));

                                        break;
                                    }
                                case element_type::symbol:
                                    {
                                        auto e_func = std::static_pointer_cast<symbol_element>(data_e);

                                        auto fn_it = function_table.find(e_func->data);


                                        if(fn_it != function_table.end()) {

                                            std::shared_ptr<symbol_entry> func
                                                ( new func_entry(name_sym_e->data, fn_it->second));

                                            symbol_table.insert(make_pair(name_sym_e->data, std::move(func)));

                                            //std::cout << "a" << std::endl;
                                        }
                                        else {
                                            std::cout << "Oooops the function is not defined" << std::endl;
                                        }

                                        break;
                                    }

                                } //case

                                // default:
                            //     {
                            //         std::cout << "not yet" << std::endl;
                            //         break;
                            //    }

                                //auto v = std::shared_ptr<var_entry>( new var_entry(name_e->, std::move(stack.pop())) );
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

                            break;
                        }


                        //         default :
                        //             {}
                        //         }
                        //         break;

                    }


                    //assert(!arg_stack.empty());
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


            template<typename Func1, typename Func2, typename Func3, typename Func4>
            auto calc(Func1 f1, Func2 f2, Func3 f3, Func4 f4) -> void
            {

                auto e2 = std::move( stack.top() );
                stack.pop();
                auto e1 = std::move( stack.top() );
                stack.pop();

                if (e1->type == element_type::integer
                    && e2->type == element_type::integer) {

                    auto e1_int = std::static_pointer_cast<int_element>(e1);
                    auto e2_int = std::static_pointer_cast<int_element>(e2);

                    std::shared_ptr<element> e3( new int_element( f1( e1_int->data, e2_int->data ) ) );
                    stack.push(std::move(e3));

                }
                else if (e1->type == element_type::real
                         && e2->type == element_type::integer) {

                    auto e1_real = std::static_pointer_cast<real_element>(e1);

                    auto e2_int = std::static_pointer_cast<int_element>(e2);

                    std::shared_ptr<element> e3( new real_element( f2(e1_real->data, e2_int->data) ) );
                    stack.push(std::move(e3));
                }
                else if (e1->type == element_type::integer
                         && e2->type == element_type::real) {


                    auto e1_int = std::static_pointer_cast<int_element>(e1);
                    auto e2_real = std::static_pointer_cast<real_element>(e2);

                    std::shared_ptr<element> e3( new real_element( f3(e1_int->data, e2_real->data) ) );
                    stack.push(std::move(e3));
                }
                else {

                    auto e1_real = std::static_pointer_cast<real_element>(e1);
                    auto e2_real = std::static_pointer_cast<real_element>(e1);

                    std::shared_ptr<element> e3( new real_element( f4(e1_real->data, e2_real->data) ) );
                    stack.push(std::move(e3));
                }
            }

        public:
            auto create_inst(std::vector<std::string> inst_vec) -> std::shared_ptr<instruction>
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

                else if (*(op_s.end() - 1 ) == ':') {
                    std::string fn_name = op_s;
                    fn_name.erase(fn_name.end() - 1);
                    return std::shared_ptr<op_instruction>
                        ( new op_instruction( operation::func, std::shared_ptr<element>
                                              ( new symbol_element( fn_name ) ) ) );
                }

                else if (op_s == "\tpush_int") {
                    return std::shared_ptr<op_instruction>
                        ( new op_instruction( operation::push_int, std::shared_ptr<element>
                                              ( new int_element( std::stoi( inst_vec[1] ) ) ) ) );
                }
                else if (op_s == "\tpush_float") {
                    return std::shared_ptr<op_instruction>
                        ( new op_instruction( operation::push_float, std::shared_ptr<element>
                                              ( new real_element( std::stof( inst_vec[1] ) ) ) ) );
                }

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
                    return std::shared_ptr<op_instruction>
                        ( new op_instruction( operation::push_symbol, create_op_element( inst_vec[1] ) ) );
                }
                else if (op_s == "\tparam") {
                    return std::shared_ptr<op_instruction>
                        ( new op_instruction( operation::param, create_op_element( inst_vec[1] ) ) );
                }
                // else if (op_s == "\tmake_clojure") {
                //     op = operation::make_clojure;
                // }

                // // op_s == "branch"
                else {
                    op = operation::branch;
                }

                return std::shared_ptr<instruction>( new instruction(op) );

            }

        public:
            auto create_op_element(const std::string operand) -> std::shared_ptr<element>
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
                    type = element_type::mte;
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
                    return std::shared_ptr<element>( new symbol_element(operand) );
                }

                return std::shared_ptr<element>( new element(type) );
            }

        };

    }
}
