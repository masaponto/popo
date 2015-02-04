#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <stack>

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


        struct operation {
        public:
            operation (std::vector<std::string> ops)
                : operate(ops[0])
            {
                if (ops.size() != 1) {
                    val = ops[1];
                }

            };

        public:
            std::string operate, val;

        };


        //enum struct element_type {num, string};
        struct elem {
        public :
            auto set_data(std::string d) -> void
            {
                data = d;
            }

            auto get_data(void) -> std::string
            {
                return data;
            }


        public:
            //element_type type;
            std::string data;
        };


        class vm
        {

        public:
            vm (std::string src)
                : ss(src) {}

        public :
            auto run() -> void
            {

                std::string str1, str2;
                std::list<std::string> code_list;

                while( std::getline(ss, str1, '\n') ) {
                    code_list.push_back(str1);
                }

                auto it = code_list.begin();
                while ( it != code_list.end() ) {

                    std::stringstream ss2(*it);

                    while( std::getline(ss2, str2, ' ') ){
                        ope.push_back(str2);
                    }

                    operation op(ope);

                    if (*(op.operate.end() - 1) == ':') {
                        std::cout << "function define" << std::endl;



                    } else {

                        stack_manager(op);

                    }

                    ope.clear();

                    ++it;
                }

            }


        public :
            auto stack_manager( operation ops ) -> void
            {

                std::string op = ops.operate;
                elem e1, e2, e3;


                if (op == "pop") {

                    stack.pop();

                } else if (op == "push") {
                    elem e;
                    e.set_data(ops.val);
                    stack.push(e);

                } else if (op == "apply") {
                    e1 = stack.top();
                    stack.pop();

                    if (e1.get_data() == "add") {
                        auto f = [](int a, int b){ return a + b; };
                        calc(stack, f);
                    }
                    else if (e1.get_data() == "sub") {
                        auto f = [](int a, int b){ return b - a; };
                        calc(stack, f);
                    }
                    else if (e1.get_data() == "mul") {
                        auto f = [](int a, int b){ return a * b; };
                        calc(stack, f);
                    }

                }


            }


            template<typename T, typename Fanc>
            auto calc(std::stack<T>, Fanc f) -> void
            {
                elem e1, e2, e3;

                e1 = stack.top();
                stack.pop();
                e2 = stack.top();
                stack.pop();

                auto num1 = std::atoi(e1.get_data().c_str());
                auto num2 = std::atoi(e2.get_data().c_str());

                e3.set_data( std::to_string( f(num1,num2) ) );
                stack.push(e3);

                std::cout << stack.top().get_data() << std::endl;
            }


        private:
            std::string line;
            std::stringstream ss;

            std::vector<std::string> ope;
            std::stack<elem> stack;
            std::list< functions<operation> > function_table;

        };


    }
}
