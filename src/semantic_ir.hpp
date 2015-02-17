#pragma once

#include <iostream>
#include <memory>
#include <list>
#include <string>
#include <algorithm>
#include <sstream>

#include "syntax.hpp"
#include "symbol_table.hpp"
// #include "ir.hpp"


namespace popo {
    namespace semantic {

    template <typename Iteratable>
    class semantic_analyzer {

    public:
//         semantic_analyzer(const Iteratable& itr)
//             : parser_(itr),
//               symbol_stack_(),
//               closure_number(0),
//               definition(),
//               consequent_number(0),
//               alternative_number(0),
//               in_define(false)

//         {
//             for (auto&& pair : special_form) {

//                 symbol_stack_.push_front(
//                     std::make_pair(pair.first,
//                                    std::shared_ptr<closure_entry>(
//                                        new closure_entry(pair.second, ""))));
//             }

//             for (auto& pair : built_in_function) {

//                 symbol_stack_.push_front(
//                     std::make_pair(pair.first,
//                                    std::shared_ptr<closure_entry>(
//                                        new closure_entry(pair.second, ""))));
//             }
//         }

        semantic_analyzer() 
            : symbol_stack_(),
              closure_number(0),
              definition(),
              consequent_number(0),
              alternative_number(0),
              in_define(false)
        {
            for (auto&& pair : special_form) {
                symbol_stack_.push_front(
                    std::make_pair(pair.first,
                                   std::shared_ptr<closure_entry>(
                                       new closure_entry(pair.second, ""))));
            }

            for (auto& pair : built_in_function) {
                symbol_stack_.push_front(
                    std::make_pair(pair.first,
                                   std::shared_ptr<closure_entry>(
                                       new closure_entry(pair.second, ""))));
            }
        }

    private:
//         syntax::s_expression_parser<Iteratable> parser_;
        std::list<std::pair<std::string, std::shared_ptr<symbol_table_entry>>>
            symbol_stack_;
        int closure_number;
        int consequent_number;
        int alternative_number;
        bool in_define;
        bool is_debug;

    public:
        std::list<std::string> definition;

    public:
        auto analyze(const Iteratable& itr, bool debug = false)
            -> std::list<std::string>
        {
            is_debug = debug;
            return analyze(syntax::s_expression_parser<Iteratable>(itr));
        }

        auto analyze(syntax::s_expression_parser<Iteratable> parser) -> std::list<std::string>
        {
            auto conscell = parser.s_exp_parse();
            if (nullptr == conscell) {
                return std::list<std::string>();
            }

            std::list<std::string> return_list;
            while (nullptr != conscell) {
                std::unique_ptr<syntax::cons_node> head_node(
                    new syntax::cons_node(std::move(conscell),
                                          std::unique_ptr<syntax::cons_node>(
                                              new syntax::cons_node())));

                auto i_list = analyze_node(std::move(head_node));
                return_list.insert(return_list.end(), i_list.begin(), i_list.end());
                return_list.push_back("write");

                conscell = parser.s_exp_parse();
            }


//             std::unique_ptr<syntax::cons_node> head_node(new syntax::cons_node(
//                 std::move(conscell),
//                 std::unique_ptr<syntax::cons_node>(new syntax::cons_node())));

//             auto i_list = analyze_node(std::move(head_node));
//             i_list.push_back("write");

//             i_list.insert(i_list.begin(), definition.begin(), definition.end());
//             i_list.remove_if([](std::string s) -> bool { return s.empty(); });
            return_list.insert(return_list.begin(), definition.begin(), definition.end());
            return_list.remove_if([](std::string s) -> bool { return s.empty(); });
            if(is_debug){
                for (auto& s : return_list) {
                    std::cout << s << std::endl;
                }
            }
            return return_list;
        }

    private:
        auto analyze_node(std::unique_ptr<syntax::expr_node> node)
            -> std::list<std::string>
        {
            std::list<std::string> s_list;
            switch (node->type) {
                case syntax::node_type::cons: {
                    auto c_list =
                        analyze_cons(cast_unique_ptr(std::move(node)));
                    s_list.insert(s_list.end(), c_list.begin(), c_list.end());
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
                    auto s = "push_bool " + t_node->to_string();
                    s_list.push_back(s);
                    //                     std::cout << s << std::endl;
                    break;
                }
                case syntax::node_type::real:{
                    auto r_node = static_cast<syntax::real_node*>(node.get());
                    auto s = "push_float " + r_node->to_string();
                    s_list.push_back(s);
                    break;

                }
                case syntax::node_type::nil: {
                    break;
                }
            }

            return s_list;
        }

        auto analyze_cons(std::unique_ptr<syntax::cons_node> cons)
            -> std::list<std::string>
        {
            std::list<std::string> s_list;
            switch (cons->car->type) {
                case syntax::node_type::cons: {
                    auto cdr_list = analyze_node(std::move(cons->cdr));
                    s_list.insert(
                        s_list.end(), cdr_list.begin(), cdr_list.end());

                    auto car = cast_unique_ptr(std::move(cons->car));

                    auto apply = "apply";
                    if (syntax::node_type::symbol == car->car->type) {
                        auto symbol_node = cast_unique_ptr<syntax::symbol_node>(
                            std::move(car->car));
                        if ("lambda" == symbol_node->val) {
                            apply = "";
                        }
                        else if("if" == symbol_node->val){
                            apply = "";
                        }
                        else if("quote" == symbol_node->val){
                            apply = "";
                        }
                        car->car.reset(symbol_node.release());
                    }
                    auto car_node =
                        cast_unique_ptr<syntax::expr_node, syntax::cons_node>(
                            std::move(car));

                    auto car_list = analyze_node(std::move(car_node));
                    s_list.insert(
                        s_list.end(), car_list.begin(), car_list.end());

                    s_list.push_back(apply);
                    break;
                }

                case syntax::node_type::symbol: {

                    // TODO
                    auto s_node = cast_unique_ptr<syntax::symbol_node>(
                        std::move(cons->car));
                    auto symbol = s_node->val;
                    cons->car.reset(
                        dynamic_cast<syntax::expr_node*>(s_node.release()));
//                     std::cout << "sym: " << symbol << std::endl;

                    assert(search_symbol_stack(symbol));
                    if (is_special_form(symbol)) {
                        return special_form_procedure(std::move(cons));
                    }

                }
                case syntax::node_type::num:
                case syntax::node_type::string:
                case syntax::node_type::real:
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

            if ("quote" == s_node->val) {
                auto quote_args_cons =
                    std::move(cast_unique_ptr(std::move(cons->cdr))->car);
                assert(syntax::node_type::cons == quote_args_cons->type);

                auto a_list = analyze_node(std::move(quote_args_cons));
                r_list.insert(r_list.end(), a_list.begin(), a_list.end());

                r_list.push_back("make_list " + std::to_string(a_list.size()));
            } else if ("lambda" == s_node->val) {

                std::list<std::string> def_tmp;
                auto cdr = cast_unique_ptr(std::move(cons->cdr));

                // function node
                auto f_node = cast_unique_ptr(std::move(cdr->cdr));
                assert(syntax::node_type::cons == f_node->car->type);

                auto func_list = analyze_node(std::move(f_node));
                func_list.insert(func_list.end(), "return");

                def_tmp.insert(
                    def_tmp.begin(), func_list.begin(), func_list.end());

                // argument node
                auto args_node = std::move(cdr->car);
                assert(syntax::node_type::cons == args_node->type);
                auto arg_list = analyze_node(std::move(args_node));
                std::for_each(
                    arg_list.begin(), arg_list.end(), [](std::string& s) {
                        s.erase(s.begin(), s.begin() + s.find(" "));
                        s.insert(0, "param");
                    });

                def_tmp.insert(
                    def_tmp.begin(), arg_list.begin(), arg_list.end());

                // label
                auto closure = "closure_" + std::to_string(closure_number++);
                def_tmp.push_front(closure + ":");

                definition.insert(
                    definition.end(), def_tmp.begin(), def_tmp.end());
                //                definition.push_back("\n");

                r_list.push_back("push_symbol " + closure);
                //r_list.push_back("push_symbol " + closure);
            } else if ("if" == s_node->val) {

                // test node
                auto test_node = std::move(cons->cdr);
                auto test_cons = cast_unique_ptr(std::move(test_node));
                auto test_list = analyze_node(
                    std::unique_ptr<syntax::cons_node>(new syntax::cons_node(
                        std::move(test_cons->car),
                        std::unique_ptr<syntax::cons_node>(
                            new syntax::cons_node()))));
                r_list.insert(
                    r_list.begin(), test_list.begin(), test_list.end());
                r_list.push_back(
                    "branch true_" + std::to_string(consequent_number) +
                    ", false_" + std::to_string(alternative_number));

                // consequent node
                auto consequent = std::move(test_cons->cdr);
                auto consequent_cons = cast_unique_ptr(std::move(consequent));
                auto conse = analyze_node(std::unique_ptr<syntax::cons_node>(
                    new syntax::cons_node(std::move(consequent_cons->car),
                                          std::unique_ptr<syntax::cons_node>(
                                              new syntax::cons_node()))));

                conse.push_front("true_" + std::to_string(consequent_number++) +
                                 ":");
                conse.push_back("return");
                definition.insert(definition.end(), conse.begin(), conse.end());

                // alternative node
                auto alternative = std::move(consequent_cons->cdr);
                auto alternative_cons = cast_unique_ptr(std::move(alternative));
                auto alter = analyze_node(std::unique_ptr<syntax::cons_node>(
                    new syntax::cons_node(std::move(alternative_cons->car),
                                          std::unique_ptr<syntax::cons_node>(
                                              new syntax::cons_node()))));

                alter.push_front("false_" +
                                 std::to_string(alternative_number++) + ":");
                alter.push_back("return");
                definition.insert(definition.end(), alter.begin(), alter.end());

            }
            else if("define" == s_node->val){
                in_define = true;
                auto cdr_node = analyze_node(std::move(cons->cdr));
                in_define = false;

                std::string symbol;
                std::stringstream ss(*cdr_node.rbegin());
                std::getline(ss, symbol, ' ');
                std::getline(ss, symbol, ' ');
//                 std::cout << "symbol: " << symbol << std::endl;

                r_list.insert(r_list.end(), cdr_node.begin(), cdr_node.end());
                r_list.insert(r_list.end(), "push_symbol define");
                symbol_stack_ .push_front(std::make_pair(symbol,
                            std::make_shared<symbol_table_entry>()));
//                 print_symbol_stack();


            }
            else {
                assert(false);
            }

            return r_list;
        }

        auto is_special_form(std::string symbol) -> bool
        {
            if ("quote" == symbol) {
                return true;
            } else if ("lambda" == symbol) {
                return true;
            }
            else if("if" == symbol) {
                return true;
            }
            else if ("define" == symbol) {
                return true;
            }
            else {
                return false;
            }
        }

        template <typename dest_type = syntax::cons_node,
                  typename src_type = syntax::expr_node>
        auto cast_unique_ptr(std::unique_ptr<src_type> node)
            -> std::unique_ptr<dest_type>
        {
            assert(nullptr != node);
            return std::unique_ptr<dest_type>(
                static_cast<dest_type*>(node.release()));
        }

        auto search_symbol_stack(const std::string& symbol) -> bool
        {
            if(in_define){
                return true;
            }
            print_symbol_stack();
            for(auto pair : symbol_stack_){
                if(symbol == pair.first){
                    return true;
                }
            }
            std::cout << symbol << " not found" << std::endl;
            return false;
        }

        auto print_symbol_stack() -> void
        {
            if(!is_debug){
                return;
            }
            std::cout << "======symbol stack======" << std::endl;
            for (auto pair : symbol_stack_) {
                std::cout << pair.first << std::endl;
            }
            std::cout << "========================" << std::endl;
        }

    };

    } // namespace semantic
} // namespace popo
