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
        s_expression_parser(const Iteratable& itr) : lex_(itr) {}

    public:
        auto s_exp_parse() -> std::unique_ptr<expr_node>
        {
            return sexp_car_parse(false);
        }

    public:
        auto get_line_number() -> int
        {
            return lex_.line_number;
        }

    private:
        auto sexp_car_parse(bool already_read_token)
            -> std::unique_ptr<expr_node>
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
                    car = std::unique_ptr<string_node>(
                        new string_node(lex_.get_lex().str));
                    break;

                case lexical::Token::num:
                    car =
                        std::unique_ptr<num_node>(
                                new num_node(lex_.get_lex().num));
                    break;

                case lexical::Token::left:
                    car = std::unique_ptr<expr_node>(sexp_car_parse(true));
                    break;

                case lexical::Token::eof:
                    if (already_read_token) {
                        assert(false);
                    }
                    return nullptr;

                case lexical::Token::right:
                    return std::unique_ptr<cons_node>(new cons_node());

                case lexical::Token::symbol:
                    car = std::unique_ptr<symbol_node>(
                        new symbol_node(lex_.get_lex().symbol));
                    break;

                // TODO add Token::t_true, t_false

                default:
                    assert(false);
            }

            return std::unique_ptr<cons_node>(
                new cons_node(std::move(car), sexp_car_parse(true)));
        }

    private:
        lexical::lexical_analyzer<Iteratable> lex_;

};


}  // namespace syntax
}  // namespace popo
