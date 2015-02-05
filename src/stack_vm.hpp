#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <stack>
#include <cassert>

namespace popo {
    namespace stack_vm {

        template<typename T>
        struct functions {
        public:
            functions (std::string f, std::list<T> c)
                : func_name(f), func_code(c) {};

        public:
            const std::string func_name;
            const std::list<T> func_code;

        };


        enum struct operate { push, main, func, param, apply, pop, end};

        struct instruction {
        public:
            instruction (std::vector<std::string> inst_vec)
            {
                const std::string op_s = inst_vec[0];

                if (op_s == "\tpop") {
                    op = operate::pop;
                } else if (op_s == "\tpush") {
                    op = operate::push;
                } else if (op_s == "\tapply") {
                    op = operate::apply;
                }  else if (op_s == "\tend") {
                    op = operate::end;
                } else if (op_s == "param") {
                    op = operate::param;
                } else if (op_s == "main:") {
                    op = operate::main;
                } else if (*(op_s.end() - 1 ) == ':') {
                    op = operate::func;
                    func_name = op_s;
                }

                if (inst_vec.size() != 1) {
                    operand = inst_vec[1];
                }

            };

        public:
            std::string operand;
            std::string func_name = "";
            operate op;

        };


        enum struct element_type {data, add, sub, mul, div};

        struct element {
        public :
            element(const std::string ope)
                :operand(ope)
            {

                if (operand == "add") {
                    type = element_type::add;
                } else if (operand == "sub") {
                    type = element_type::sub;
                } else if (operand == "mul") {
                    type = element_type::mul;
                } else if (operand == "div") {
                    type = element_type::div;
                } else {
                    type = element_type::data;
                }

            }

        public:
            auto get_data(void) -> std::string
            {
                return operand;
            }




        // public:
        //     auto operator + (element e) -> element
        //     {
        //         // assert(this->type != element_type::data
        //         //        || e.type != element_type::data);


        //         auto num1 = 1;
        //         auto num2 = 2;


        //         // auto num1 = std::stoi(this->ope);
        //         // auto num2 = std::stoi(e.ope);
        //         element e_(std::to_string( num1 + num2 ));

        //         return e_;
        //     }


        public:
            element_type type;
            const std::string operand;
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

                    instruction inst(inst_vec);

                    inst_list.push_back(inst);

                    inst_vec.clear();
                }


                run(inst_list);

            }

        public:
            auto run(const std::list<instruction> inst_list) -> void
            {

                auto it = inst_list.begin();
                while ( it != inst_list.end() ) {

                    // if (it -> operate = "main:") {
                    // }


                    // TODO: change to case sentence

                    switch(it->op) {
                    case operate::main:
                        {
                            std::cout << "main function" << std::endl;
                            break;
                        }

                    case operate::func:
                        {
                            std::cout << "function define" << std::endl;

                            const std::string func_name = it->func_name;
                            std::list<instruction> func_code;

                            while(it->op != operate::end) {
                                func_code.push_back(*it);
                                ++it;
                            }

                            const functions<instruction> func(func_name, func_code);
                            function_table.push_back(func);

                            std::cout << func.func_name << std::endl;
                            break;
                        }

                    default:
                        {
                            stack_manager(*it);
                        }

                    }

                    // if (it->op == operate::main) {
                    //     std::cout << "main function" << std::endl;

                    //     //while it*

                    // } else if (it->op == operate::func) {
                    //     std::cout << "function define" << std::endl;

                    //     const std::string func_name = it->func_name;
                    //     std::list<instruction> func_code;

                    //     while(it->op != operate::end) {
                    //         func_code.push_back(*it);
                    //         ++it;
                    //     }

                    //     const functions<instruction> func(func_name, func_code);
                    //     function_table.push_back(func);

                    //     std::cout << func.func_name << std::endl;

                    // }
                    // else {
                    //     stack_manager(*it);

                    // }

                    ++it;
                }
            }



        public :
            auto stack_manager(const instruction inst ) -> void
            {

                switch(inst.op) {

                case operate::pop :
                    {
                        stack.pop();
                        break;
                    }

                case operate::push :
                    {
                        element e(inst.operand);
                        stack.push(e);
                        break;
                    }

                case operate::apply :
                    {
                        element e_ = stack.top();
                        stack.pop();

                        // enum struct element_type {data, add, sub, mul, div};

                        switch(e_.type) {

                        case element_type::add :
                            {
                                // element e1 = stack.top();
                                // stack.pop();

                                // element e2 = stack.top();
                                // stack.pop();

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

                        default :
                            {


                            }

                        }
                        break;
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

                auto num1 = std::atoi(e1.get_data().c_str());
                auto num2 = std::atoi(e2.get_data().c_str());

                element e3( std::to_string( f(num1,num2) ) );
                stack.push(e3);

                std::cout << stack.top().get_data() << std::endl;
            }


        };

    }
}
