#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <stack>
#include <cassert>

namespace popo {
    namespace stack_vm {

        struct var {
        public:
            var(std::string l, std::string d) : label(l), data(d) {}
        public:
            std::string label, data;
        };

        template<typename T>
        struct functions {
        public:
            functions (std::string f, std::list<T> c)
                : name(f), code(c) {};

        public:
            const std::string name;
            std::list<T> code;
            std::list<var> var_table;
            std::stack<std::string> var_stack;
        };

        enum struct element_type {data, add, sub, mul, div, counter, func};

        struct element {
        public :
            element() {}
            element(const element_type t) :type(t) {}
            element(const element_type t, std::string o) :type(t), operand(o) {}

        public:
            element_type type;
            std::string operand;
        };

        // public:
        //    auto operator + (std::unique_ptr<element> p) -> std::unique_ptr<element>
        //    {
        //         // assert(this->type != element_type::data
        //         //        || e.type != element_type::data);


        //        //auto num1 = 1;
        //        //auto num2 = 2;


        //        // auto num1 = std::stoi(this->ope);
        //        // auto num2 = std::stoi(e.ope);

        //        //element e_(std::to_string( num1 + num2));
        //        //element e_("ababaa");

        //        return p;
        //    }


        enum struct operate { push, main, func, param, apply, pop, ret };

        struct instruction {
        public:
            instruction (operate o) :op(o) {}
            instruction (operate o, element e) :op(o), elm(e) {}
            instruction (operate o, std::string n) :op(o), name(n) {}

        public:
            operate op;
            element elm;
            std::string name;
        };

        // struct operation : element
        // {
        // public :
        //     operation(const std::string ope) : element();
        // };
        // struct program_counter: element
        // {
        // public :
        //     program_counter(std::list<instruction>::iterator c)
        //         : pc(c)
        //     {
        //         type = element_type::counter;
        //     }

        // public :
        //     const std::list<instruction>::iterator pc;

        // };


        class vm
        {
        public:
            vm (std::string ir_code)
                : ir_code_ss(ir_code) {}

        private:
            std::stringstream ir_code_ss;
            std::list<instruction> inst_list;
            std::stack<element> stack;
            std::list< functions<instruction> > function_table;

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
                else if (op_s == "\tret") {
                    op = operate::ret;
                }
                else if (op_s == "main:") {
                    op = operate::main;
                }
                else if (*(op_s.end() - 1 ) == ':') {
                    op = operate::func;
                    std::string fn_name = op_s;
                    fn_name.erase(fn_name.end() - 1);
                    return instruction(op, fn_name);
                }
                else if (op_s == "\tpush") {
                    op = operate::push;
                    return instruction(op, create_element(inst_vec[1]));
                }
                else if (op_s == "\tparam") {
                    op = operate::param;
                    return instruction(op, inst_vec[1]);
                }
                else {
                    //std::assert();
                }

                return instruction(op);

            }

        public:
            auto create_element(const std::string operand) -> element
            {
                element_type type;

                // for (auto it = function_table.begin(); it != function_table.end(); ++it)
                //     {
                //         std::cout << "aa" << std::endl;
                //         std::cout << it->name << std::endl;
                //         if (operand == it->name) {
                //             return element(element_type::func);
                //         }
                //     }

                if (operand == "add") {
                    type = element_type::add;
                }
                else if (operand == "sub") {
                    type = element_type::sub;
                }
                else if (operand == "mul") {
                    type = element_type::mul;
                }
                else if (operand == "div") {
                    type = element_type::div;
                }
                else {
                    return element(element_type::data, operand);
                }

                return element(type);
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

                            const functions<instruction> func(func_name, func_code);
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
                case operate::push :
                    {
                        //if (arg_stack)
                        stack.push(inst.elm);
                        break;
                    }
                case operate::apply :
                    {
                        const element e_ = stack.top();
                        stack.pop();

                        // enum struct element_type {data, add, sub, mul, div};

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
                        default :
                            {}
                        }
                        break;
                    }


                    // case operate::param :
                    //     {
                    //         arg_stack.push(stack.top());
                    //         stack.pop();
                    //         break;
                    //     }

                    assert(!arg_stack.empty());
                }
            }

            auto run_function(functions<instruction> func) -> void
            {
                for(auto inst_it = func.code.begin();
                    inst_it != func.code.end(); ++inst_it) {

                    if (inst_it->op == operate::param) {

                        var v(inst_it->name, stack.top().operand);
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

                std::cout << stack.top().operand << std::endl;
                //stack.pop();
            }

        };

    }
}
