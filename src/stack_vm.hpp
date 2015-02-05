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
                : func_name(f), code(c) {};


            // public:
            //     auto add_function(std::list<string> ope, list<string>::iterator it) -> void
            //     {


            //     }


        public:
            std::string func_name;
            std::list<T> code;

        };


        enum struct operate { push, main, func, param, apply, pop };

        struct instruction {
        public:
            instruction (std::vector<std::string> inst_vec)
            {

                std::string op_s = inst_vec[0];

                if (op_s == "\tpop") {
                    op = operate::pop;

                } else if (op_s == "\tpush") {
                    op = operate::push;

                } else if (op_s == "\tapply") {
                    op = operate::apply;

                } else if (op_s == "main:") {
                    op = operate::main;
                }


                if (inst_vec.size() != 1) {
                    operand = inst_vec[1];
                }

            };

        public:
            std::string operand;
            operate op;

        };


        enum struct element_type {data, add, sub, mul, div};

        struct element {
        public :
            element(std::string operand)
            {
                ope = operand;

                if (ope == "add") {
                    type = element_type::add;

                } else if (ope == "sub") {
                    type = element_type::sub;

                } else if (ope == "mul") {
                    type = element_type::mul;

                } else if (ope == "div") {
                    type = element_type::div;

                } else {
                    type = element_type::data;
                }

            }

        public:
            auto get_data(void) -> std::string
            {
                return ope;
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
            std::string ope;
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
            auto run(std::list<instruction> inst_list) -> void
            {

                auto it = inst_list.begin();
                while ( it != inst_list.end() ) {

                    // if (it -> operate = "main:") {
                    // }

                    if (it->op == operate::main) {
                        std::cout << "function define" << std::endl;

                        //while it*

                    } else {
                        stack_manager(*it);

                    }

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
