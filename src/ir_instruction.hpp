#include <memory>

namespace popo {
    namespace stack_vm {

        enum struct element_type {
            integer, real, string, boolean, list,
                int_list, real_list,
                add, sub, mul, div, eq,
                lt, mt, lte, mte, land, lor,
                cons, car, cdr,
                symbol, data, define
                };


        struct element {
        public :
            element(element_type t) :type(t) {}

            virtual ~element() {}

        public:
            element_type type;
        };

        struct int_element : element {
        public:
            int_element(int op) : element(element_type::integer), data(op) {}
        public:
            int data;
        };

        struct real_element : element {
        public:
            real_element(float op) : element(element_type::real), data(op) {}
        public:
            float data;
        };

        struct string_element : element {
        public:
            string_element(std::string op) : element(element_type::string), data(op) {}
        public:
            std::string data;
        };

        struct bool_element : element {
        public:
            bool_element(bool op) : element(element_type::boolean), data(op) {}
        public:
            bool data;
        };

        // template<typename T>
        // struct list_element : element {
        // public:
        //     list_element(std::list<T> op) : element(element_type::list), data(op) {}
        // public:
        //     std::list<T> data;
        // };


        struct int_list_element : element {
        public:
            int_list_element(std::list<int> op) : element(element_type::int_list), data(op) {}
        public:
            std::list<int> data;
        };

        struct real_list_element : element {
        public:
            real_list_element(std::list<float> op) : element(element_type::real_list), data(op) {}
        public:
            std::list<float> data;
        };


        struct symbol_element : element {
        public:
            symbol_element(std::string op) : element(element_type::symbol), data(op) {}
        public:
            std::string data;
        };


        enum struct operation {
            push_int, push_float, push_string, push_symbol, push_bool, push_list,
                pop, main, func, param, apply, ret, make_clojure, branch,
                read, write, make_list
                };

        struct instruction {
        public:
           instruction (operation o) :op(o) {}
            virtual ~instruction(){}

        public:
            operation op;
        };

        struct op_instruction : instruction {
        public:
            op_instruction(operation o, std::shared_ptr<element> e)
                : instruction(o)
            {
                operand = std::move(e);
            }

        public :
            std::shared_ptr<element> operand;
        };


    }
}
