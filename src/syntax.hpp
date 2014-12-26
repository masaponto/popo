#pragma once

#include <iostream>
#include <cassert>
#include <memory>
#include "lexical.hpp"
#include "node.hpp"

namespace popo {
namespace syntax{

template <typename Iteratable>
class s_expression_parser {
public:
    s_expression_parser(const Iteratable& itr) : lex_(itr){}

public:
    auto s_exp_parse() -> std::unique_ptr<expr_node>
    {
        is_fail = false;
        return sexp_car_parse(false);
    }

private:
    auto sexp_car_parse(bool already_read_token) -> std::unique_ptr<expr_node>
    {
        if (!already_read_token) {
            auto token = lex_.get_next_token();
            if (lexical::Token::eof == token) {
                return nullptr;
            }
            assert(lexical::Token::left == token);
        }
        std::unique_ptr<expr_node> car;
        switch (lex_.get_next_token()) {
            case lexical::Token::string:
                car = std::move(std::unique_ptr<string_node>(
                    new string_node(lex_.get_lex().str)));
                break;

            case lexical::Token::num:
                car = std::move(std::unique_ptr<num_node>(
                    new num_node(lex_.get_lex().num)));
                break;

            case lexical::Token::left:
                car =
                    std::move(std::unique_ptr<expr_node>(sexp_car_parse(true)));
                break;

            case lexical::Token::eof:
                if(already_read_token){
                    assert(false);
                }
                return nullptr;

            case lexical::Token::right:
                return std::unique_ptr<cons_node>(new cons_node());

            default:
                assert(false);
        }

        return std::unique_ptr<cons_node>(
            new cons_node(std::move(car), sexp_car_parse(true)));
    }

private:
    lexical::lexical_analyser<Iteratable> lex_;
    bool is_fail;

};


}  // namespace syntax
}  // namespace popo
