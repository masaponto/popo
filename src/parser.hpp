#pragma once

#include <iostream>
#include <cassert>
#include "lexer.hpp"

namespace popo {
    namespace parser {

        enum struct node_type{
            num,
            string,
            symbol,
            cons
        };

        class expr_node
        {
            public:
                expr_node(node_type t) :type(t){}
                virtual ~expr_node() {}

            public:
                node_type type;

        };

        class num_node
            : public expr_node
        {
            public:
                num_node(int value)
                    : expr_node(node_type::num), val(value) {}

            public:
                int val;
        };

        class string_node
            : public expr_node
        {
            public:
                string_node(std::string value)
                    :expr_node(node_type::string), val(value){}

            public:
                std::string val;
        };

        class symbol_node
            : public expr_node
        {
            public:
                symbol_node(std::string value)
                    : expr_node(node_type::symbol), val(value){}

            public:
                std::string val;
        };

        class cons_node
            : public expr_node
        {
            public:
                cons_node(): expr_node(node_type::cons) {
                    car = nullptr;
                    cdr = nullptr;
                }

                cons_node(expr_node* ca, expr_node* cd)
                    : expr_node(node_type::cons), car(ca), cdr(cd){}

            public:
                expr_node* car;
                expr_node* cdr;
        };

        template<typename Iteratable>
        class s_expression_parser
        {
            public:
                s_expression_parser(lexer::lexical_analyser<Iteratable>& lex)
                    : lex_(lex) {}

            public:
                auto sexp_parse() -> expr_node*;
//                 {
//                     return sexp_car_parse(false);
//                 }

            private:
                auto sexp_car_parse(bool already_read_token)
                    -> expr_node*
                {
                    auto cons = new cons_node();
                    if(!already_read_token){
                        assert(lexer::Token::left == lex_.get_next_token());
                    }
                    switch(lex_.get_next_token()){
                        case lexer::Token::string:
                            cons->car = new string_node(lex_.get_lex().str);
                            break;

                        case lexer::Token::num:
                            cons->car = new num_node(lex_.get_lex().num);
                            break;

                        default:
                            assert(false);
                    }

                    cons->cdr = sexp_cdr_parse(lexer::Token::eof);
                    return cons;
                }

                auto sexp_cdr_parse(lexer::Token next_token)
                    -> expr_node*
                {
                    auto cons = new cons_node();
                    if(lexer::Token::eof == next_token){
                        next_token = lex_.get_next_token();
                    }
                    switch(next_token){
                        case lexer::Token::string:
                            cons->car = new string_node(lex_.get_lex().str);
                            break;

                        case lexer::Token::num:
                            cons->car = new num_node(lex_.get_lex().num);
                            break;

                        case lexer::Token::right:
                            delete cons;
                            return s_expression_parser::nil;

                        default:
                            assert(false);
                    }

                    auto t = lex_.get_next_token();
                    if(lexer::Token::left == t){
                        cons->cdr = sexp_car_parse(true);
                    }
                    else {
                        cons->cdr = sexp_cdr_parse(t);
                    }
                    return cons;
                }

            private:
                lexer::lexical_analyser<Iteratable> lex_;

            public:
                static cons_node* nil;

        };

//         cons_node* s_expression_parser::nil = new cons_node();

        auto print_cons(expr_node* node)
            -> void
        {
//             static auto i = 0;
//             if(node->type == node_type::num){
//                 auto n = static_cast<num_node*>(node);
//                 std::cout << i++ << " " << n->val << std::endl;
//             }
//             else if(node->type == node_type::string){
//                 auto n = static_cast<string_node*>(node);
//                 std::cout << i++ << " " << n->val << std::endl;
//             }
//             else if(node->type == node_type::cons){
//                 auto n = static_cast<cons_node*>(node);
//                 std::cout << i++ << std::endl;
//                 if(s_expression_parser::nil == n){

//                 }
//                 print_cons(n->car);
//                 print_cons(n->cdr);
//             }

            return;
        }

    } // namespace parser
} // namespace popo
