#pragma once
#include <string>
#include <vector>
#include <list>



namespace popo {
    namespace ir {
    // Three address code

        enum struct ir_type { label, jmp, assignment, operation };
        struct three_addr_code_base {
            public:
                three_addr_code_base(ir_type t) : type(t) {};

            public:
                ir_type type;
        };

        // test:
        struct label : public three_addr_code_base {
            public:
                label(std::string s)
                    : three_addr_code_base(ir_type::label), str(s) {};

            public:
                std::string str;
        };

        // goto test
        struct jmp : public three_addr_code_base {
            public:
                jmp(std::string l)
                    : three_addr_code_base(ir_type::jmp), label(l) {};

            public:
                std::string label;
        };

        // t0 = t1 + t2
        struct assignment : public three_addr_code_base {

            enum struct operation{add, sub, mul, div};
            public:
                assignment(int right, int left, operation o)
                    : three_addr_code_base(ir_type::assignment),
                    rvalue_r_num(right), lvalue_r_num(left), op(o) {};

            public:
                int rvalue_r_num;
                int lvalue_r_num;
                operation op;
        };

        //TODO: 条件分岐

        class ir_manager {

            public:
                ir_manager() : registers(), instruction() {};


            private:
                std::vector<int> registers;
                std::list<three_addr_code_base> instruction;
        };
    }
} // namespace popo
