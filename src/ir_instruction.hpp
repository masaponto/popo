#include <memory>

namespace popo {
    namespace stack_vm {

        enum struct element_type {
            integer, real, string, boolean, list,
                add, sub, mul, div, eq,
                lt, mt, lte, mte, land, lor,
                cons, car, cdr,
                branch, symbol, data, define
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

        struct list_element : element {
        public:
            list_element( std::list<std::shared_ptr<element>> e_list) : element(element_type::list), data(e_list) {}
         public:
            std::list<std::shared_ptr<element>> data;
        };

        struct symbol_element : element {
        public:
            symbol_element(std::string op) : element(element_type::symbol), data(op) {}
        public:
            std::string data;
        };

        struct branch_element : element {
        public:
            branch_element(std::string op1, std::string op2)
                : element(element_type::branch), t_label(op1), f_label(op2) {}
        public:
            std::string t_label, f_label;
        };


        enum struct operation {
            push_int, push_float, push_string, push_symbol, push_bool, push_list,
                pop, main, func, param, apply, ret, make_clojure, branch,
                read, write, make_list, other
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
