#pragma once

#include <string>
#include <cassert>
#include <vector>
#include <list>
#include <utility>
#include "syntax.hpp"
#include "symbol_table.hpp"

//TODO: change procedure, when popo find error.
//      now: assert -> feture: exception(?) or the other.

namespace popo {
namespace semantic {

    template <typename Iteratable>
    class semantic_analyzer {

    public:
        semantic_analyzer(const Iteratable& itr) : parser_(itr), symbol_stack()
        {
            for (auto& pair : special_form) {
                symbol_stack.table_stack.push_front(
                    std::make_pair(pair.first,
                                   std::make_shared<function_entry>(
                                       pair.second.argument_num)));
            }

            for (auto& pair : built_in_function) {
                symbol_stack.table_stack.push_front(
                    std::make_pair(pair.first,
                                   std::make_shared<function_entry>(
                                       pair.second.argument_num)));
            }
        };

    public:
        static std::pair<std::string, std::shared_ptr<symbol_table_entry>>
            not_found_pair;
        static std::shared_ptr<symbol_table_entry> dummy_entry;

    public:
        auto analyze() -> std::shared_ptr<symbol_table_entry>
        {

            auto conscell = parser_.s_exp_parse();
            if (nullptr == conscell) {
                return nullptr;
            }

            return analyze_cons(cast_unique_ptr(std::move(conscell)));
        }

    private:
        auto get_car_depth(syntax::cons_node* cons) -> int
        {
            assert(nullptr != cons);
            auto cdr = static_cast<syntax::cons_node*>(cons->cdr.get());
            if (syntax::node_type::nil == cdr->type) {
                return 0;
            } else {
                return get_car_depth(cdr) + 1;
            }
        }

        template <typename n_type>
        auto regist_value_entry(
            std::string symbol,
            //                     std::unique_ptr<syntax::cons_node>&&
            // value_node)
            std::unique_ptr<syntax::cons_node> value_node)
            -> std::shared_ptr<symbol_table_entry>
        {

            auto node = cast_unique_ptr<n_type>(std::move(value_node->car));

            std::shared_ptr<symbol_table_entry> entry =
                std::make_shared<value_entry>(node->val, node->type);

            symbol_stack.table_stack.push_front(std::make_pair(symbol, entry));

            return entry;
        }

        auto define_procedure(std::unique_ptr<syntax::expr_node> cons)
            -> std::shared_ptr<symbol_table_entry>
        {
            // TODO: this function is push to table_stack and regist to
            // symbol_table.

            auto cons_node = cast_unique_ptr(std::move(cons));

            // check symbol
            assert(syntax::node_type::symbol == cons_node->car->type);

            auto symbol_node = cast_unique_ptr<syntax::symbol_node>(
                              std::move(cons_node->car));

            std::cout << "define: " << symbol_node->val << std::endl;

            auto value_cons_node = cast_unique_ptr(std::move(cons_node->cdr));

            switch (value_cons_node->car->type) {
                case syntax::node_type::cons: {

                    auto symbol_ptr = analyze_cons(
                        cast_unique_ptr(std::move(value_cons_node->car)));

                    symbol_stack.table_stack.push_front(
                        std::make_pair(symbol_node->val, symbol_ptr));

                    return symbol_ptr;
                }
                case syntax::node_type::num:
                    return regist_value_entry<syntax::num_node>(
                        symbol_node->val, std::move(value_cons_node));

                case syntax::node_type::string:
                    return regist_value_entry<syntax::string_node>(
                        symbol_node->val, std::move(value_cons_node));

                case syntax::node_type::symbol:
                    return regist_value_entry<syntax::symbol_node>(
                        symbol_node->val, std::move(value_cons_node));

                case syntax::node_type::nil: {
                    std::shared_ptr<symbol_table_entry> entry =
                        std::make_shared<list_entry>(
                            std::unique_ptr<syntax::cons_node>(
                                new syntax::cons_node()));

                    symbol_stack.table_stack.push_front(
                        std::make_pair(symbol_node->val, entry));

                    return entry;
                }
                case syntax::node_type::trust: {
                    return regist_value_entry<syntax::trust_node>(
                        symbol_node->val, std::move(value_cons_node));
                }

                default:
                    assert(false);
            }
        }

        auto push_arguments(const syntax::cons_node* cons) -> int
        {
            if (syntax::node_type::nil == cons->type) {
                return 0;
            }
            const auto car = cons->car.get();
            assert(syntax::node_type::symbol == car->type);
            auto symbol = static_cast<syntax::symbol_node*>(car)->val;
            symbol_stack.table_stack.push_front(
                std::make_pair(symbol, nullptr));
            symbol_stack.local_symbol_num++;

            return push_arguments(
                       static_cast<const syntax::cons_node*>(cons->cdr.get())) +
                   1;
        }

        auto lambda_procedure(std::unique_ptr<syntax::expr_node> cons)
            -> std::shared_ptr<symbol_table_entry>
        {
            auto cons_node = cast_unique_ptr(std::move(cons));

            auto function_cons = cast_unique_ptr(
                std::move(cast_unique_ptr(std::move(cons_node->cdr))->car));

            auto argument_cons = cast_unique_ptr(std::move(cons_node->car));


            // push dummy argument to symbol stack
            auto argument_count = push_arguments(argument_cons.get());
            std::cout << argument_count << std::endl;

            // TODO: which do we have to use static_pointer_cast or
            // dynamic_pointer_cast
            auto entry = std::static_pointer_cast<
                function_entry>(
                //                 std::dynamic_pointer_cast<function_entry>(
                analyze_cons(std::move(function_cons)));

//             entry->argument_num = argument_count;

            // pop dummy argumeny at symbol stack
            for (int i = 0; i < symbol_stack.local_symbol_num; ++i) {
                symbol_stack.table_stack.pop_front();
            }
            symbol_stack.local_symbol_num -= argument_count;

            return entry;
        }

        auto quote_procedure(std::unique_ptr<syntax::expr_node> cons)
            -> std::shared_ptr<symbol_table_entry>
        {
            std::cout << "quote" << std::endl;

            auto cons_node = cast_unique_ptr(std::move(cons));

            auto list_node = cast_unique_ptr(std::move(cons_node->car));

            std::shared_ptr<symbol_table_entry> entry =
                std::make_shared<list_entry>(std::move(list_node));

            return entry;
        }

        auto if_procedure(std::unique_ptr<syntax::expr_node> cons)
            -> std::shared_ptr<symbol_table_entry>
        {
            std::cout << "if" << std::endl;

            auto cons_node = cast_unique_ptr(std::move(cons));

            auto test = cast_unique_ptr(std::move(cons_node->car));

            auto exec_node = cast_unique_ptr(std::move(cons_node->cdr));

            auto consequent = cast_unique_ptr(std::move(exec_node->car));

            auto alternative = cast_unique_ptr(
                std::move(cast_unique_ptr(std::move(exec_node->cdr))->car));

            auto test_entry = analyze_cons(std::move(test));

            auto consequent_entry = analyze_cons(std::move(consequent));

            auto alternative_entry = analyze_cons(std::move(alternative));

            return dummy_entry;
        }

        auto check_argument(std::unique_ptr<syntax::cons_node> cons,
                int last_arg_num)
            -> void
        {
            if (syntax::node_type::nil == cons->type) {
                if (0 == last_arg_num) {
                    return;
                } else {
                    assert(false);
                }
            } else if (last_arg_num <= 0) {
                assert(false);
            }

            if (syntax::node_type::cons == cons->car->type) {
                // TODO return value of analyze_cons
                analyze_cons(cast_unique_ptr(std::move(cons->car)));
            }

            return check_argument(cast_unique_ptr(std::move(cons->cdr)),
                                  last_arg_num - 1);
        }

        auto other_procedure(std::unique_ptr<syntax::cons_node> cons)
            -> std::shared_ptr<symbol_table_entry>
        {
            if (syntax::node_type::cons == cons->car->type) {
                auto func_entry =
                    std::static_pointer_cast<function_entry>(
                    analyze_cons(cast_unique_ptr(std::move(cons->car))));

                check_argument(cast_unique_ptr(std::move(cons->cdr)),
                               func_entry ->argument_num);

                return func_entry;

            }
            else if (syntax::node_type::symbol == cons->car->type) {

                auto symbol_node =
                    cast_unique_ptr<syntax::symbol_node>(std::move(cons->car));

                std::cout << symbol_node->val << std::endl;

                auto& pair = search_function(symbol_node->val);
                assert(not_found_pair != pair);

                auto func_entry = std::static_pointer_cast<function_entry>(pair.second);

                // when you call function of argument at higher-order function
                if(nullptr == func_entry){
                    return nullptr;
                }

                // check function definition
                assert(entry_type::function == func_entry->type_);

                auto arg_num = func_entry->argument_num;
                std::cout << arg_num << std::endl;

                check_argument(cast_unique_ptr(std::move(cons->cdr)), arg_num);

                return func_entry;
            } else {
                assert(false);
            }

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

        auto analyze_cons(std::unique_ptr<syntax::cons_node> cons)
            -> std::shared_ptr<symbol_table_entry>
        {
            assert(nullptr != cons);
            print_symbol_stack();

            if (syntax::node_type::symbol == cons->car->type) {

                auto function_name =
                    static_cast<syntax::symbol_node*>(cons->car.get())->val;

                switch (divide_function(function_name)) {
                    case function_type::sf_define:
                        return define_procedure(std::move(cons->cdr));

                    case function_type::sf_lambda:
                        return lambda_procedure(std::move(cons->cdr));

                    case function_type::sf_quote:
                        return quote_procedure(std::move(cons->cdr));

                    case function_type::sf_if:
                        return if_procedure(std::move(cons->cdr));

                    case function_type::other:
                        return other_procedure(std::move(cons));

                    default:
                        assert(false);
                }

            } else if (syntax::node_type::cons == cons->car->type) {
                return other_procedure(std::move(cons));
            }
            else {
                assert(false);
            }
        }

        //             auto analyze_cons(std::unique_ptr<syntax::cons_node>
        // cons)
        //                 -> std::shared_ptr<symbol_table_entry>
        //             {
        //                 assert(nullptr != cons);
        ////                 node type check
        //                 assert(syntax::node_type::symbol == cons->car->type);

        //                 auto function_name =
        //                     static_cast<syntax::string_node*>(cons->car.get())->val;

        //                 auto& pair = search_function(function_name);

        ////               check function definition
        //                 assert(entry_type::function ==
        //                        static_cast<function_entry*>(pair.second.get())->type_);

        //                 auto arg_num =
        //                     static_cast<function_entry*>(pair.second.get())->argument_num;

        ////                argument check of number
        //                 assert(get_car_depth(cons.get()) == arg_num);

        //                 switch (divide_function(function_name)) {
        //                     case function_type::sf_define:
        //                         return
        // define_procedure(std::move(cons->cdr));

        //                     case function_type::sf_lambda:
        //                         return
        // lambda_procedure(std::move(cons->cdr));

        //                     case function_type::sf_quote:
        //                         return quote_procedure(std::move(cons->cdr));

        //                     case function_type::sf_if:
        //                         return if_procedure(std::move(cons->cdr));

        //                     case function_type::other:
        //                         std::cout << "other: " << function_name <<
        // std::endl;
        //                         return
        // std::make_shared<function_entry>(std::move(cons));

        //                     default:
        //                         assert(false);
        //                 }
        //             }

        auto search_function(std::string func_name)
            -> std::pair<std::string, std::shared_ptr<symbol_table_entry>>&
        {
            for (auto& data : symbol_stack.table_stack) {
                if (data.first == func_name) {
                    return data;
                }
            }

            return not_found_pair;
        }

        template <typename type = syntax::cons_node>
        auto cast_unique_ptr(std::unique_ptr<syntax::expr_node> node)
            -> std::unique_ptr<type>
        {
            assert(nullptr != node);
            return std::unique_ptr<type>(static_cast<type*>(node.release()));
        }


        auto print_symbol_stack() -> void
        {
            std::cout << "--- symbol stack ---" << std::endl;
            for (auto& data : symbol_stack.table_stack) {
                std::cout << "symbol name: " << data.first << " ";
                if (nullptr != data.second) {
                    if (entry_type::function == data.second->type_) {
                        auto argc = std::static_pointer_cast<function_entry>(
                                        data.second)->argument_num;

                        std::cout << argc;
                    }
                }
                std::cout << std::endl;
            }
        }

    private:
        syntax::s_expression_parser<Iteratable> parser_;

        struct symbol_table_stack {
        public:
            symbol_table_stack() : table_stack(), local_symbol_num(0) {};

        public:
            std::list<
                std::pair<std::string, std::shared_ptr<symbol_table_entry>>>
                table_stack;
            int local_symbol_num;
        } symbol_stack;
    };

    template<typename T>
    std::pair<std::string, std::shared_ptr<symbol_table_entry>>
        semantic_analyzer<T>::not_found_pair = std::make_pair("",
            std::make_shared<symbol_table_entry>());

    template<typename T>
    std::shared_ptr<symbol_table_entry> semantic_analyzer<T>::dummy_entry =
        std::make_shared<symbol_table_entry>();

}  // namespace semantic
}  // namespace popo
