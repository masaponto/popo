#pragma once

#include <iostream>

// program = sexp
// sexp = atom | ( slist | quote sexp
// slist = sexp slist | )

namespace popo {
    namespace parser {

        class expr_node
        {
            public:
                virtual ~expr_node(){}
        };

        class num_node
            : public expr_node
        {
            public:
                num_node(int value) : val(value){}

            public:
                int val;
        };

        class string_node
            : public expr_node
        {
            public:
                string_node(std::string value) :val(value){}

            public:
                std::string val;
        };

        class symbol_node
            : public expr_node
        {
            public:
                symbol_node(std::string value) : val(value){}

            public:
                std::string val;
        };

        class cons_node
            : public expr_node
        {
            public:
                cons_node() {
                    car_ = nullptr;
                    cdr_ = nullptr;
                }

                cons_node(expr_node* car, expr_node* cdr)
                    : car_(car), cdr_(cdr){}

            public:
                expr_node* car_;
                expr_node* cdr_;
        };

//         class function_node
//             : expr_node
//         {
//             public
//                 function_node()
//         }


    } // namespace parser
} // namespace popo
