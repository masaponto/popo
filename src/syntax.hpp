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

auto get_space_string(int num) -> std::unique_ptr<std::string>
{
    std::unique_ptr<std::string> spaces(new std::string('|', 0));
    for (int i = 0; i < num; i++) {
        *spaces += "|";
    }
    return spaces;
}

template <typename T>
auto _print_cons(std::unique_ptr<expr_node> node, int depth, bool is_func)
    -> void
{
    if (node->type == node_type::nil) {
        return;
    } else if (node->type == node_type::num) {
        std::unique_ptr<num_node> vn(dynamic_cast<num_node*>(node.release()));
        std::cout << *get_space_string(depth) << vn->val << std::endl;
    } else if (node->type == node_type::string) {
        std::unique_ptr<string_node> vn(
            dynamic_cast<string_node*>(node.release()));

        if (is_func) {
            std::cout << *get_space_string(depth - 1) << '+' << std::flush;
        } else {
            std::cout << *get_space_string(depth) << std::flush;
        }
        std::cout << vn->val << std::endl;
    } else if (node->type == node_type::cons) {
        std::unique_ptr<cons_node> n(dynamic_cast<cons_node*>(node.release()));
        if (node_type::cons == n->car->type) {
            _print_cons<T>(std::move(n->car), depth + 1, true);
        } else {
            _print_cons<T>(std::move(n->car), depth, is_func);
        }
        _print_cons<T>(std::move(n->cdr), depth, false);
    }
}

template <typename T>
auto print_cons(std::unique_ptr<expr_node> node) -> void
{
    std::unique_ptr<cons_node> nil(new cons_node());
    std::unique_ptr<cons_node> new_root(
        new cons_node(std::move(node), std::move(nil)));
    _print_cons<T>(std::move(new_root), 0, true);
}

}  // namespace syntax
}  // namespace popo
