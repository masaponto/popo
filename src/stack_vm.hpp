#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <stack>
#include <cassert>

#include "ir_instruction.hpp"

namespace popo {
    namespace stack_vm {

        struct function {
        public:
            function (std::string f, std::list<instruction> c)
                : name(f), code(c) {};

        public:
            const std::string name;
            std::list<instruction> code;
            std::list<var> var_table;
            std::stack<std::string> var_stack;
        };


        class vm
        {
        public:
            vm (std::string ir_code)
                : ir_code_ss(ir_code) {}

        private:
            std::stringstream ir_code_ss;
            std::list<instruction> inst_list;
            std::stack<element> stack;
            std::list<function> function_table;
            std::stack<function> function_stack;
            std::list<var> var_table;

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

                    inst_list.push_back( create_inst(inst_vec) );
                    inst_vec.clear();
                }

                run(inst_list);
            }


        public:
            auto run(const std::list<instruction> inst_list) -> void
            {

                auto it = inst_list.begin();
                while ( it != inst_list.end() ) {
                    switch(it->op) {
                    case operate::main:
                        {
                            std::cout << "main function" << std::endl;
                            break;
                        }
                    case operate::func:
                        {
                            std::cout << "function define" << std::endl;

                            const std::string func_name = it->name;
                            std::list<instruction> func_code;

                            while(it->op != operate::ret) {
                                func_code.push_back(*it);
                                ++it;
                            }

                            const function func(func_name, func_code);
                            function_table.push_back(func);

                            std::cout << func.name << std::endl;
                            break;
                        }
                    default:
                        {
                            stack_manager(*it);
                        }

                    }
                    ++it;
                }
            }

        public :
            auto stack_manager(const instruction inst ) -> void
            {
                std::stack<element> arg_stack;

                switch(inst.op) {
                case operate::ret :
                    {
                        break;
                    }
                case operate::pop :
                    {
                        arg_stack.push(stack.top());
                        stack.pop();
                        break;
                    }
                case operate::write :
                    {
                        if (!stack.empty()) {
                            std::cout << stack.top().operand << std::endl;
                        } else {
                            std::cout << "Ooops, stack is empty :(" << std::endl;
                        }
                    }
                case operate::push_int :
                case operate::push_float :
                case operate::push_string :
                case operate::push_bool :
                case operate::push_list :
                    {
                        stack.push(inst.elm);
                        break;
                    }
                case operate::push_symbol :
                    {
                        bool find_flag = false;

                        for(auto v_it = var_table.begin();
                            v_it != var_table.end(); ++v_it) {
                            if (v_it->label == inst.elm.operand ) {
                                stack.push(element(v_it->type, v_it->data));
                                find_flag = true;
                                break;
                            }
                        }
                        if(!find_flag) {
                            stack.push(inst.elm);
                        }
                        break;
                    }

                case operate::apply :
                    {
                        const element e_ = stack.top();
                        stack.pop();

                        switch(e_.type) {
                        case element_type::add :
                            {
                                // element e1 = stack.top();
                                // stack.pop();
                                // element e2 = stack.top();
                                // stack.pop();

                                // auto po = std::unique_ptr<element>(e2);

                                // std::cout << "abbbabba" << std::endl;
                                // stack.push(e1+e2);

                                auto add = [](int a, int b){ return a + b; };
                                calc(stack, add);
                                break;
                            }
                        case element_type::sub :
                            {
                                auto sub = [](int a, int b){ return b - a; };
                                calc(stack, sub);
                                break;
                            }
                        case element_type::mul :
                            {
                                auto mul = [](int a, int b){ return a * b; };
                                calc(stack, mul);
                                break;
                            }
                        case element_type::data :
                            {
                                for(auto it = function_table.begin(); it != function_table.end(); ++it)
                                    {
                                        if (it->name == e_.operand) {
                                            std::cout << "function " << e_.operand << " is called " << std::endl;
                                            run_function(*it);
                                        }
                                    }
                                break;
                            }
                        case element_type::define :
                            {
                                element label_e = stack.top();
                                stack.pop();
                                element data_e = stack.top();
                                stack.pop();

                                var v(label_e.operand, data_e.operand, data_e.type);

                                var_table.push_back(v);
                                std::cout << "define var " << v.label << "  "<< v.data << std::endl;

                                // for(auto it = inst_list.begin();
                                //     it != inst_list.end(); ++it) {
                                //     if (it->elm.operand == v.label ) {
                                //         std::cout << "aa" << std::endl;
                                //         it->op =
                                //         it->elm = element(data_e.type, v.data);
                                //     }
                                // }

                                break;
                            }


                        default :
                            {}
                        }
                        break;
                    }

                    assert(!arg_stack.empty());
                }
            }

            auto run_function(function func) -> void
            {
                for(auto inst_it = func.code.begin();
                    inst_it != func.code.end(); ++inst_it) {

                    if (inst_it->op == operate::param) {

                        var v(inst_it->name, stack.top().operand, stack.top().type);
                        stack.pop();
                        func.var_table.push_back(v);

                        for(auto it = func.code.begin();
                            it != func.code.end(); ++it) {

                            if (it->elm.operand == v.label ) {
                                it->elm = element(element_type::data, v.data);
                            }
                        }


                    } else {
                        stack_manager(*inst_it);
                    }
                }
            }


            template<typename T, typename Fanc>
            auto calc(std::stack<T>, Fanc f) -> void
            {
                element e1 = stack.top();
                stack.pop();
                element e2 = stack.top();
                stack.pop();

                auto num1 = std::atoi(e1.operand.c_str());
                auto num2 = std::atoi(e2.operand.c_str());

                element e3(element_type::data, std::to_string( f(num1,num2) ) );
                stack.push(e3);
            }


        public:
            auto create_inst(std::vector<std::string> inst_vec) -> instruction
            {
                const std::string op_s = inst_vec[0];
                operate op;

                if (op_s == "\tpop") {
                    op = operate::pop;
                }
                else if (op_s == "\tapply") {
                    op = operate::apply;
                }
                else if (op_s == "\treturn") {
                    op = operate::ret;
                }
                else if (op_s == "\tread") {
                    op = operate::read;
                }
                else if (op_s == "\twrite") {
                    op = operate::write;
                }
                else if (op_s == "main:") {
                    op = operate::main;
                }
                else if (*(op_s.end() - 1 ) == ':') {
                    std::string fn_name = op_s;
                    fn_name.erase(fn_name.end() - 1);
                    return instruction(operate::func, fn_name);
                }

                else if (op_s == "\tpush_int") {
                    return instruction(operate::push_int, element(element_type::integer, inst_vec[1]));
                }
                else if (op_s == "\tpush_float") {
                    return instruction(operate::push_float, element(element_type::real, inst_vec[1]));
                }
                else if (op_s == "\tpush_string") {
                    return instruction(operate::push_string, element(element_type::string, inst_vec[1]));
                }
                else if (op_s == "\tpush_bool") {
                    return instruction(operate::push_bool, element(element_type::boolean, inst_vec[1]));
                }
                else if (op_s == "\tpush_list") {
                    return instruction(operate::push_list, element(element_type::list, inst_vec[1]));
                }
                else if (op_s == "\tpush_symbol") {
                    return instruction(operate::push_symbol, create_op_element(inst_vec[1]));
                }
                else if (op_s == "\tparam") {
                    op = operate::param;
                }
                else if (op_s == "\tmake_clojure") {
                    op = operate::make_clojure;
                }
                // op_s == "branch"
                else {
                    op = operate::branch;
                }

                return instruction(op);

            }

        public:
            auto create_op_element(const std::string operand) -> element
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
                    type = element_type::func;
                    return element(type, operand);
                }
                return element(type);
            }

        };

    }
}
