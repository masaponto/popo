#pragma once

#include <iostream>

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
                    car = nullptr;
                    cdr = nullptr;
                }

                cons_node(expr_node* ca, expr_node* cd)
                    : car(ca), cdr(cd){}

            public:
                expr_node* car;
                expr_node* cdr;
        };

//         class function_node
//             : expr_node
//         {
//             public
//                 function_node()
//         }

        template<typename Iteratable>
        class s_expression_parser
        {
            public:
                s_expression_parser(lexer::lexical_analyser<Iteratable>& lex)
                    : lex_(lex), nil() {}

            public:
                auto parse_sexp()
                    -> expr_node*
                {
                    lexer::Token token = lex_.get_next_token();
                    cons_node *cons = new cons_node();
                    if(lexer::Token::left == token){
                        if(lexer::Token::string == lex_.get_next_token()){
                            cons->car = new string_node(lex_.get_lex().str);
                        }
                        else{
                            goto syntax_error;
                        }
                    }
                    else{
                        goto syntax_error;
                    }

                    return cons;

syntax_error:
                    delete cons;
                    return nullptr;
                }


            private:
                lexer::lexical_analyser<Iteratable> lex_;
                cons_node* nil;
        };

    } // namespace parser
} // namespace popo
