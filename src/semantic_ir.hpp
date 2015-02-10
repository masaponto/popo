#pragma once

#include <iostream>
#include <memory>
#include <list>
#include <string>

#include "syntax.hpp"
#include "symbol_table.hpp"
#include "ir.hpp"


namespace popo {
    namespace semantic {

    template <typename Iteratable>
    class semantic_analyzer {

    public:
        semantic_analyzer(const Iteratable& itr)
            : parser_(itr), symbol_stack_(), ir_manager_()
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
        }

    private:
        syntax::s_expression_parser<Iteratable> parser_;
        std::list<std::pair<std::string, std::shared_ptr<symbol_table_entry>>>
            symbol_stack_;
        ir::ir_manager ir_manager_;

    public:
        auto analyze() -> std::list<std::string>
        {

            auto conscell = parser_.s_exp_parse();
            if (nullptr == conscell) {
                return std::list<std::string>();
            }
            std::unique_ptr<syntax::cons_node> head_node(
                    new syntax::cons_node(
                        std::move(conscell),
                        std::unique_ptr<syntax::cons_node>(new syntax::cons_node())));

            return analyze_node(std::move(head_node));
        }

    private:
        auto analyze_node(std::unique_ptr<syntax::expr_node> node)
            -> std::list<std::string>
        {
            std::list<std::string> s_list;
            switch (node->type) {
                case syntax::node_type::cons: {
                    auto c_list = analyze_cons(cast_unique_ptr(std::move(node)));
                    s_list.insert(
                        s_list.end(), c_list.begin(), c_list.end());
                    break;
                }
                case syntax::node_type::num: {
                    auto n_node = static_cast<syntax::num_node*>(node.get());
                    auto s = "push_int " + n_node->to_string();
//                     std::cout << s << std::endl;
                    s_list.push_back(s);
                    break;
                }
                case syntax::node_type::string: {
                    auto s_node = static_cast<syntax::string_node*>(node.get());
                    auto s = "push_string " + s_node->val;
//                     std::cout << s << std::endl;
                    s_list.push_back(s);
                    break;
                }
                case syntax::node_type::symbol: {
                    auto s_node = static_cast<syntax::symbol_node*>(node.get());
                    auto s = "push_symbol " + s_node->val;
//                     std::cout << s << std::endl;
                    s_list.push_back(s);
                    break;
                }
                case syntax::node_type::trust: {
                    auto t_node = static_cast<syntax::trust_node*>(node.get());
                    auto s = "push_trust " + t_node->to_string();
                    s_list.push_back(s);
//                     std::cout << s << std::endl;
                    break;
                }
                case syntax::node_type::nil: {
                    break;
                }
            }

            return s_list;
        }

        auto analyze_cons(std::unique_ptr<syntax::cons_node> cons) -> std::list<std::string>
        {
            std::list<std::string> s_list;
            switch (cons->car->type) {
                case syntax::node_type::cons: {
                    auto cdr_list = analyze_node(std::move(cons->cdr));
                    s_list.insert(
                        s_list.end(), cdr_list.begin(), cdr_list.end());
                    auto car_list = analyze_node(std::move(cons->car));
                    s_list.insert(
                        s_list.end(), car_list.begin(), car_list.end());
                    s_list.push_back("apply");
                    break;
                }

                case syntax::node_type::num:
                case syntax::node_type::string:
                case syntax::node_type::symbol: {

                    //TODO
                    auto s_node = cast_unique_ptr<syntax::symbol_node>(
                        std::move(cons->car));
                    auto symbol = s_node->val;
                    cons->car.reset(
                            dynamic_cast<syntax::expr_node*>(s_node.release()));

                    if (is_special_form(symbol)) {
                        return special_form_procedure(std::move(cons));
                    }
                }
                case syntax::node_type::trust: {
                    auto cdr_list = analyze_node(std::move(cons->cdr));
                    s_list.insert(
                        s_list.end(), cdr_list.begin(), cdr_list.end());
                    auto car_list = analyze_node(std::move(cons->car));
                    s_list.insert(
                        s_list.end(), car_list.begin(), car_list.end());
                    break;
                }
                case syntax::node_type::nil:
                    assert(false);
            }
            return s_list;
        }

        auto special_form_procedure(std::unique_ptr<syntax::cons_node> cons)
            -> std::list<std::string>
        {
            std::list<std::string> r_list;

            auto s_node =
                cast_unique_ptr<syntax::symbol_node>(std::move(cons->car));

            if("quote" == s_node->val){
                auto quote_args_cons = std::move(cast_unique_ptr(std::move(cons->cdr))->car);
                assert(syntax::node_type::cons == quote_args_cons->type);

                auto a_list = analyze_node(std::move(quote_args_cons));
                r_list.insert(
                        r_list.end(), a_list.begin(), a_list.end());

                r_list.push_back("make_list " + std::to_string(a_list.size()));
            }
//             else if("define" == s_node->val){

//             }
            else {
                assert(false);
            }

            return r_list;

        }

        auto is_special_form(std::string symbol)
            -> bool
        {
            if ("quote" == symbol) {
                return true;
            }
//             else if ("if" == symbol) {
//                 return true;
//             }
            else {
                return false;
            }
        }

        template <typename dest_type = syntax::cons_node,
                  typename src_type = syntax::expr_node>
        auto cast_unique_ptr(std::unique_ptr<src_type> node) -> std::unique_ptr<dest_type>
        {
            assert(nullptr != node);
            return std::unique_ptr<dest_type>(static_cast<dest_type*>(node.release()));
        }
    };

    } // namespace semantic
} // namespace popo
