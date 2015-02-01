#pragma once

#include <string>
#include <cassert>
#include <vector>
#include <list>
#include <utility>
#include "syntax.hpp"
#include "symbol_table.hpp"


namespace popo {
    namespace semantic {

    template <typename Iteratable>
    class semantic_analyzer {

    public:
        semantic_analyzer(const Iteratable& itr) : parser_(itr), symbol_stack_()

        {
            for (auto pair : special_form) {
                symbol_stack_.push_front(
                    std::make_pair(pair.first, pair.second));
            }

            for (auto pair : built_in_function) {
                symbol_stack_.push_front(
                    std::make_pair(pair.first, pair.second));
            }
        };

    public:
        auto analyze() -> std::unique_ptr<syntax::expr_node>
        {

            auto conscell = parser_.s_exp_parse();
            if (nullptr == conscell) {
                return nullptr;
            }

            analyze_cons(static_cast<const syntax::cons_node*>(conscell.get()));
            return std::move(conscell);
        }

    private:
        auto analyze_cons(const syntax::cons_node* cons)
            -> std::pair<std::string, int>
        {
            assert(nullptr != cons);
            print_symbol_stack();

            if (syntax::node_type::symbol == cons->car->type) {

                auto function_name =
                    static_cast<syntax::symbol_node*>(cons->car.get())->val;

                std::cout << function_name << std::endl;

                switch (divide_function(function_name)) {
                    case function_type::sf_define:
                        return define_procedure(
                            static_cast<syntax::cons_node*>(cons->cdr.get()));

                    case function_type::sf_lambda:
                        return lambda_procedure(
                            static_cast<syntax::cons_node*>(cons->cdr.get()));

                    case function_type::sf_quote:
                        return quote_procedure(
                            static_cast<syntax::cons_node*>(cons->cdr.get()));

                    case function_type::sf_if:
                        return if_procedure(
                            static_cast<syntax::cons_node*>(cons->cdr.get()));

                    case function_type::other:
                        return other_procedure(cons);

                    default:
                        assert(false);
                }

            } else if (syntax::node_type::cons == cons->car->type) {

                analyze_cons(static_cast<syntax::cons_node*>(cons->car.get()));
                analyze_cons(static_cast<syntax::cons_node*>(cons->cdr.get()));
            } else {
                assert(false);
            }
            return not_found_pair;
        }

        auto define_procedure(const syntax::cons_node* cons)
            -> std::pair<std::string, int>
        {
            // check symbol
            assert(syntax::node_type::symbol == cons->car->type);

            auto symbol_node =
                static_cast<syntax::symbol_node*>(cons->car.get());

            auto value_cons_node =
                static_cast<syntax::cons_node*>(cons->cdr.get());

            auto argc = -1;
            if (syntax::node_type::cons == value_cons_node->car->type) {
                argc = analyze_cons(static_cast<syntax::cons_node*>(
                                        value_cons_node->car.get())).second;
            }
            symbol_stack_.push_front(std::make_pair(symbol_node->val, argc));

            assert(syntax::node_type::nil == value_cons_node->cdr->type);

            return not_found_pair;
        }

        auto lambda_procedure(const syntax::cons_node* cons)
            -> std::pair<std::string, int>
        {

            assert(syntax::node_type::cons == cons->car->type);

            auto func_cons = static_cast<syntax::cons_node*>(cons->cdr.get());

            auto argc = push_arguments(
                static_cast<syntax::cons_node*>(cons->car.get()));

            assert(syntax::node_type::cons == func_cons->car->type);
            assert(syntax::node_type::nil == func_cons->cdr->type);

            return std::make_pair("", argc);
        }

        auto quote_procedure(const syntax::cons_node* cons)
            -> std::pair<std::string, int>

        {
            assert(syntax::node_type::cons == cons->car->type);
            assert(syntax::node_type::nil == cons->cdr->type);

            return std::make_pair("", -1);
        }

        auto if_procedure(const syntax::cons_node* cons)
            -> std::pair<std::string, int>
        {
            assert(syntax::node_type::cons == cons->car->type);

            auto test = static_cast<syntax::cons_node*>(cons->car.get());
            analyze_cons(test);

            auto cdr = static_cast<syntax::cons_node*>(cons->cdr.get());

            assert(syntax::node_type::cons == cdr->car->type);
            auto consequent = static_cast<syntax::cons_node*>(cdr->car.get());
            analyze_cons(consequent);

            cdr = static_cast<syntax::cons_node*>(cdr->cdr.get());
            assert(syntax::node_type::cons == cdr->car->type);

            auto alternative = static_cast<syntax::cons_node*>(cdr->car.get());
            analyze_cons(alternative);

            assert(syntax::node_type::nil == cdr->cdr->type);

            return not_found_pair;
        }

        auto other_procedure(const syntax::cons_node* cons)
            -> std::pair<std::string, int>
        {
            assert(syntax::node_type::symbol == cons->car->type);

            auto symbol =
                static_cast<syntax::symbol_node*>(cons->car.get())->val;

            auto pair = search_symbol(symbol);
            if (pair.second >= 0) {
                auto argc = get_argument_num(
                    static_cast<syntax::cons_node*>(cons->cdr.get()));

                assert(argc == pair.second);
            }

            return not_found_pair;
        }

        auto search_symbol(std::string func_name) -> std::pair<std::string, int>
        {
            for (auto& data : symbol_stack_) {
                if (data.first == func_name) {
                    return data;
                }
            }

            return not_found_pair;
        }

        auto push_arguments(const syntax::cons_node* cons) -> int
        {
            if (syntax::node_type::nil == cons->type) {
                return 0;
            }

            assert(syntax::node_type::symbol == cons->car->type);
            auto symbol =
                static_cast<syntax::symbol_node*>(cons->car.get())->val;

            symbol_stack_.push_front(std::make_pair(symbol, -1));

            return push_arguments(
                       static_cast<const syntax::cons_node*>(cons->cdr.get())) +
                   1;
        }

        auto get_argument_num(const syntax::cons_node* cons) -> int
        {
            if (syntax::node_type::nil == cons->type) {
                return 0;
            }

            if (syntax::node_type::cons == cons->car->type) {
                analyze_cons(static_cast<syntax::cons_node*>(cons->car.get()));
            } else if (syntax::node_type::symbol == cons->car->type) {
                auto symbol =
                    static_cast<syntax::symbol_node*>(cons->car.get())->val;

                assert(not_found_pair != search_symbol(symbol));
            }
            //             const auto car = cons->car.get();
            //             assert(syntax::node_type::symbol == car->type);
            //             auto symbol =
            // static_cast<syntax::symbol_node*>(car)->val;

            return get_argument_num(
                       static_cast<const syntax::cons_node*>(cons->cdr.get())) +
                   1;
        }

        template <typename type = syntax::cons_node,
                  typename src_type = std::unique_ptr<syntax::expr_node>>
        auto cast_unique_ptr(src_type node) -> std::unique_ptr<type>
        {
            assert(nullptr != node);
            return std::unique_ptr<type>(static_cast<type*>(node.release()));
        }

        auto divide_function(std::string str) -> function_type
        {
            // TODO: add special form
            if ("define" == str) {
                return function_type::sf_define;
            } else if ("lambda" == str) {
                return function_type::sf_lambda;
            } else if ("quote" == str) {
                return function_type::sf_quote;
            } else if ("if" == str) {
                return function_type::sf_if;
            } else {
                return function_type::other;
            }
        }

        auto print_symbol_stack() -> void
        {
            std::cout << "--- symbol stack ---" << std::endl;
            for (auto&& data : symbol_stack_) {
                std::cout << "symbol name: " << data.first << " ";
                if (-1 != data.second) {
                    std::cout << data.second;
                }
                std::cout << std::endl;
            }
        }

    public:
        static std::pair<std::string, int> not_found_pair;
        static std::shared_ptr<symbol_table_entry> dummy_entry;

    private:
        syntax::s_expression_parser<Iteratable> parser_;
        std::list<std::pair<std::string, int>> symbol_stack_;
    };

    template <typename T>
    std::pair<std::string, int> semantic_analyzer<T>::not_found_pair =
        std::make_pair("", -1);

    }  // namespace semantic
} // namespace popo
