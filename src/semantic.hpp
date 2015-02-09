#pragma once

#include <string>
#include <cassert>
#include <vector>
#include <list>
#include <utility>
#include "syntax.hpp"
#include "symbol_table.hpp"
#include "ir.hpp"


namespace popo {
    namespace semantic {

    template <typename Iteratable>
    class semantic_analyzer {

    public:
        semantic_analyzer(const Iteratable& itr)
            : ir_manager(), parser_(itr), symbol_stack_()

        {
            for (auto&& pair : special_form) {

                symbol_stack_.push_front(
                    std::make_pair(pair.first,
                                   std::shared_ptr<clojure_entry>(
                                       new clojure_entry(pair.second, ""))));
            }

            for (auto& pair : built_in_function) {

                symbol_stack_.push_front(
                    std::make_pair(pair.first,
                                   std::shared_ptr<clojure_entry>(
                                       new clojure_entry(pair.second, ""))));
            }
        };

    public:
        auto analyze() -> std::shared_ptr<syntax::expr_node>
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
            -> std::pair<std::string, std::shared_ptr<symbol_table_entry>>
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
            } else if(syntax::node_type::num == cons->car->type) {
//                 static_cast<syntax::num_node*>(cons->car.get());
            }
            else {
                assert(false);
            }
            return not_found_pair;
        }

        auto define_procedure(const syntax::cons_node* cons)
            -> std::pair<std::string, std::shared_ptr<symbol_table_entry>>
        {
            // check symbol
            assert(syntax::node_type::symbol == cons->car->type);

            auto symbol_node =
                static_cast<syntax::symbol_node*>(cons->car.get());
//             ir_manager.define_enable(symbol_node->val);
//             ir_manager.define_enable();

            auto value_cons_node =
                static_cast<syntax::cons_node*>(cons->cdr.get());

            // TODO have to rewrite below code to switch.
            if (syntax::node_type::cons == value_cons_node->car->type) {
                //                 auto clojure = static_cast<clojure_entry*>(
                //                     analyze_cons(static_cast<syntax::cons_node*>(
                //                                      value_cons_node->car.get())).second.get());
                auto entry =
                    analyze_cons(static_cast<syntax::cons_node*>(
                                     value_cons_node->car.get())).second;

                symbol_stack_.push_front(
                    std::make_pair(symbol_node->val, entry));
            } else if (syntax::node_type::num == value_cons_node->car->type) {

                auto num_node =
                    static_cast<syntax::num_node*>(value_cons_node->car.get());

                symbol_stack_.push_front(std::make_pair(
                    symbol_node->val,
                    std::make_shared<value_entry>(num_node->val)));

            } else if (syntax::node_type::symbol ==
                       value_cons_node->car->type) {

                auto symbol = static_cast<syntax::symbol_node*>(
                    value_cons_node->car.get());

                symbol_stack_.push_front(std::make_pair(
                    symbol_node->val,
                    std::make_shared<value_entry>(symbol->val,
                                                  syntax::node_type::symbol)));
            } else if (syntax::node_type::string ==
                       value_cons_node->car->type) {
            }

            assert(syntax::node_type::nil == value_cons_node->cdr->type);

//             ir_manager.define_disable();
            return not_found_pair;
        }

        auto lambda_procedure(const syntax::cons_node* cons)
            -> std::pair<std::string, std::shared_ptr<symbol_table_entry>>
        {
            assert(syntax::node_type::cons == cons->car->type);

            auto func_cons = static_cast<syntax::cons_node*>(cons->cdr.get());


            assert(syntax::node_type::cons == func_cons->car->type);
            assert(syntax::node_type::nil == func_cons->cdr->type);

            auto argc = push_arguments(
                    static_cast<syntax::cons_node*>(cons->car.get()));

            auto entry = analyze_cons(
                static_cast<syntax::cons_node*>(func_cons->car.get())).second;

            return std::make_pair("",
                                  std::make_shared<clojure_entry>(argc, ""));
        }

        auto quote_procedure(const syntax::cons_node* cons)
            -> std::pair<std::string, std::shared_ptr<symbol_table_entry>>

        {
            assert(syntax::node_type::cons == cons->car->type);
            assert(syntax::node_type::nil == cons->cdr->type);

            // TODO make quote symbol table entry struct
            return std::make_pair("", std::make_shared<value_entry>(-1));
        }

        auto if_procedure(const syntax::cons_node* cons)
            -> std::pair<std::string, std::shared_ptr<symbol_table_entry>>
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
            -> std::pair<std::string, std::shared_ptr<symbol_table_entry>>
        {
            assert(syntax::node_type::symbol == cons->car->type);

            auto symbol =
                static_cast<syntax::symbol_node*>(cons->car.get())->val;

            auto pair = search_symbol(symbol);
            if (pair.second >= 0) {
                auto argc = get_argument_num(
                    static_cast<syntax::cons_node*>(cons->cdr.get()));

                auto clojure = static_cast<clojure_entry*>(pair.second.get());
//                 assert(argc == clojure->arg_count);
            }

            return not_found_pair;
        }

        auto search_symbol(std::string func_name)
            -> std::pair<std::string, std::shared_ptr<symbol_table_entry>>
        {
            for (auto& data : symbol_stack_) {
                if (data.first == func_name) {
                    if (entry_type::value == data.second->type) {
                        auto v_entry =
                            static_cast<value_entry*>(data.second.get());

                        if (syntax::node_type::symbol == v_entry->node_type) {
                            return search_symbol(v_entry->string_value);
                        }
                    }

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

            //             symbol_stack_.push_front(std::make_pair(symbol, -1));
            symbol_stack_.push_front(
                std::make_pair(symbol,
                               std::make_shared<value_entry>(
                                   symbol, syntax::node_type::string)));

            auto argc = push_arguments(static_cast<const syntax::cons_node*>(
                            cons->cdr.get())) +
                        1;
//             ir_manager.push_params(symbol);

            return argc;
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

                auto pair = search_symbol(symbol);
                assert(not_found_pair != pair);
                switch (pair.second->type) {
                    case entry_type::value: {
                        auto v_entry =
                            static_cast<value_entry*>(pair.second.get());

                        switch (v_entry->node_type) {
                            case syntax::node_type::num:
//                                 ir_manager.push_args(
//                                     std::to_string(v_entry->int_value));
                                break;

                            case syntax::node_type::string:
//                                 ir_manager.push_args(v_entry->string_value);
                                break;
                        }

                        break;
                    }
                    case entry_type::clojure:
//                         ir_manager.push_args(search_symbol(symbol).first);
                        break;
                }

            } else {
//                 ir_manager.push_args(cons->car.get()->to_string());
            }
            //             const auto car = cons->car.get();
            //             assert(syntax::node_type::symbol == car->type);
            //             auto symbol =
            // static_cast<syntax::symbol_node*>(car)->val;

            auto argc = get_argument_num(static_cast<const syntax::cons_node*>(
                            cons->cdr.get())) +
                        1;

            return argc;
        }

        template <typename type = syntax::cons_node,
                  typename src_type = std::shared_ptr<syntax::expr_node>>
        auto cast_unique_ptr(src_type node) -> std::shared_ptr<type>
        {
            assert(nullptr != node);
            return std::shared_ptr<type>(static_cast<type*>(node.release()));
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
                if (entry_type::value == data.second->type) {
                    auto v_entry = static_cast<value_entry*>(data.second.get());
                    if (syntax::node_type::num == v_entry->node_type) {
                        std::cout << v_entry->int_value;
                    } else if (syntax::node_type::symbol ==
                                   v_entry->node_type ||
                               syntax::node_type::string ==
                                   v_entry->node_type) {
                        std::cout << v_entry->string_value;
                    }
                } else if (entry_type::clojure == data.second->type) {
                }

                std::cout << std::endl;
            }
        }

    public:
        static std::pair<std::string, std::shared_ptr<symbol_table_entry>>
            not_found_pair;
        static std::shared_ptr<symbol_table_entry> dummy_entry;
        ir::ir_manager ir_manager;

    private:
        syntax::s_expression_parser<Iteratable> parser_;
        //         std::list<std::pair<std::string, int>> symbol_stack_;
        std::list<std::pair<std::string, std::shared_ptr<symbol_table_entry>>>
            symbol_stack_;
    };

    template <typename T>
    std::pair<std::string, std::shared_ptr<symbol_table_entry>>
        semantic_analyzer<T>::not_found_pair = std::make_pair(
            "",
            std::make_shared<symbol_table_entry>());

    }  // namespace semantic
} // namespace popo
