#pragma once

#include <iostream>
#include <cassert>
#include <memory>
#include "lexer.hpp"

namespace popo {
namespace parser {

enum struct node_type { num, string, symbol, cons, nil };

class expr_node {
public:
    expr_node(node_type t) : type(t) {}
    virtual ~expr_node() {}

public:
    node_type type;
    expr_node* parent_node;
};

class num_node : public expr_node {
public:
    num_node(int value) : expr_node(node_type::num), val(value) {}

public:
    int val;
};

class string_node : public expr_node {
public:
    string_node(std::string value) : expr_node(node_type::string), val(value) {}

public:
    std::string val;
};

class symbol_node : public expr_node {
public:
    symbol_node(std::string value) : expr_node(node_type::symbol), val(value) {}

public:
    std::string val;
};

class cons_node : public expr_node {
public:
    cons_node() : expr_node(node_type::nil)
    {
        car = nullptr;
        cdr = nullptr;
    }

    cons_node(std::unique_ptr<expr_node> ca, std::unique_ptr<expr_node> cd)
        : expr_node(node_type::cons)
    {
        car = std::move(ca);
        cdr = std::move(cd);
    }

public:
    std::unique_ptr<expr_node> car;
    std::unique_ptr<expr_node> cdr;
};

template <typename Iteratable>
class s_expression_parser {
public:
    s_expression_parser(lexer::lexical_analyser<Iteratable>& lex) : lex_(lex) {}

public:
    auto s_exp_parse() -> std::unique_ptr<expr_node>
    {
        return sexp_car_parse(false);
    }

private:
    auto sexp_car_parse(bool already_read_token) -> std::unique_ptr<expr_node>
    {
        if (!already_read_token) {
            auto token = lex_.get_next_token();
            if (lexer::Token::eof == token) {
                return nullptr;
            }
            assert(lexer::Token::left == token);
        }
        std::unique_ptr<expr_node> car;
        switch (lex_.get_next_token()) {
            case lexer::Token::string:
                car = std::move(std::unique_ptr<string_node>(
                    new string_node(lex_.get_lex().str)));
                break;

            case lexer::Token::num:
                car = std::move(std::unique_ptr<num_node>(
                    new num_node(lex_.get_lex().num)));
                break;

            case lexer::Token::left:
                car =
                    std::move(std::unique_ptr<expr_node>(sexp_car_parse(true)));
                break;

            case lexer::Token::eof:
                return nullptr;

            case lexer::Token::right:
                return std::unique_ptr<cons_node>(new cons_node());

            default:
                assert(false);
        }

        return std::unique_ptr<cons_node>(
            new cons_node(std::move(car), sexp_car_parse(true)));
    }

private:
    lexer::lexical_analyser<Iteratable> lex_;

public:
};

auto get_space_string(int num) -> std::unique_ptr<std::string>
{
    std::unique_ptr<std::string> spaces(new std::string(' ', 0));
    for (int i = 0; i < num; i++) {
        *spaces += " ";
    }
    return spaces;
}

template <typename T>
auto _print_cons(std::unique_ptr<expr_node> node, int depth) -> void
{
    if (node->type == node_type::nil) {
        return;
    } else if (node->type == node_type::num) {
        auto vn = dynamic_cast<num_node*>(node.release());
        std::cout << *get_space_string(depth) << vn->val << std::endl;
    } else if (node->type == node_type::string) {
        auto vn = dynamic_cast<string_node*>(node.release());
        std::cout << *get_space_string(depth) << vn->val << std::endl;
    } else if (node->type == node_type::cons) {
        auto n = dynamic_cast<cons_node*>(node.release());
        if (node_type::cons == n->car->type) {
            _print_cons<T>(std::move(n->car), depth + 1);
        } else {
            _print_cons<T>(std::move(n->car), depth);
        }
        _print_cons<T>(std::move(n->cdr), depth);
    }
}

template <typename T>
auto print_cons(std::unique_ptr<expr_node> node) -> void
{
    _print_cons<T>(std::move(node), 0);
}

}  // namespace parser
}  // namespace popo
