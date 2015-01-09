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
    class semantic_analyser {

        public:
            semantic_analyser(const Iteratable& itr)
                : parser_(itr), symbol_stack(), function_table()
            {
                for (auto&& pair : special_form) {
                    symbol_stack.table_stack.push_front(
                        std::make_pair(pair.first,
                                       std::unique_ptr<symbol_table_entry>(
                                           new function_entry(pair.second))));
                }

                for (auto&& pair : built_in_function) {
                    symbol_stack.table_stack.push_front(
                        std::make_pair(pair.first,
                                       std::unique_ptr<symbol_table_entry>(
                                           new function_entry(pair.second))));
                }

                analyse();
            };

        public:
            static std::pair<std::string, std::unique_ptr<symbol_table_entry>> not_found_pair;

        private:
            enum struct function_type {
                DEFINE,
                LAMBDA,
                OTHER,
            };

        private:

            auto get_argument_number(syntax::cons_node* cons)
                -> int
            {
                assert(nullptr != cons);
                auto cdr = static_cast<syntax::cons_node*>(cons->cdr.get());
                if(syntax::node_type::nil == cdr->type){
                    return 0;
                }
                else {
                    return get_argument_number(cdr) + 1;
                }
            }

            template<typename n_type>
            auto regist_value_entry(std::string symbol,
                    std::unique_ptr<syntax::cons_node> value_node)
                -> void
            {
                auto node = std::unique_ptr<n_type>(
                    static_cast<n_type*>(value_node->car.release()));

                auto entry = std::unique_ptr<value_entry>(
                    new value_entry(node->val, node->type));

                symbol_stack.table_stack.push_front(
                    std::make_pair(symbol, std::move(entry)));
            }

            auto define_procedure(std::unique_ptr<syntax::expr_node>&& cons)
                -> std::unique_ptr<syntax::cons_node>
            {
                // TODO: this function is push to table_stack and regist to
                // symbol_table.

                auto cons_node = std::unique_ptr<syntax::cons_node>(
                    static_cast<syntax::cons_node*>(cons.release()));

                // check symbol
                assert(syntax::node_type::symbol == cons_node->car->type);
                auto symbol =
                    std::unique_ptr<syntax::symbol_node>(
                        static_cast<syntax::symbol_node*>(cons_node->car.release()))
                        ->val;


                std::cout << "define: " << symbol << std::endl;
                auto value_cons_node = std::unique_ptr<syntax::cons_node>(
                    static_cast<syntax::cons_node*>(cons_node->cdr.release()));

                switch (value_cons_node->car->type) {
                    case syntax::node_type::cons:
                    {

                        auto c_node =
                            analyse_cons(std::unique_ptr<syntax::cons_node>(
                                static_cast<syntax::cons_node*>(
                                    value_cons_node->car.release())));

                        auto entry = std::unique_ptr<function_entry>(
                            new function_entry(get_argument_number(
                                c_node.get()), function_table.size()));
                        function_table.push_back(std::move(c_node));

                        break;
                    }
                    case syntax::node_type::num:
                        regist_value_entry<syntax::num_node>(
                            symbol, std::move(value_cons_node));
                        break;

                    case syntax::node_type::string:
                        regist_value_entry<syntax::string_node>(
                            symbol, std::move(value_cons_node));
                        break;

                    case syntax::node_type::symbol:
                        regist_value_entry<syntax::symbol_node>(
                            symbol, std::move(value_cons_node));
                        break;

                    case syntax::node_type::nil: {
                        // TODO write code
                        break;
                    }
                }

                return nullptr;
            }

            auto lambda_procedure(std::unique_ptr<syntax::expr_node>&& cons)
                -> std::unique_ptr<syntax::cons_node>
            {
                std::cout << "lambda" <<  std::endl;
                auto cons_node = std::unique_ptr<syntax::cons_node>(
                    static_cast<syntax::cons_node*>(cons.release()));

                return cons_node;
            }

            auto divide_function(std::string str)
                -> function_type
            {
                //TODO: add special form
                if("define" == str){
                    return function_type::DEFINE;
                }

                else if("lambda" == str) {
                    return function_type::LAMBDA;
                }

                else {
                    return function_type::OTHER;
                }
            }

            auto analyse() -> void
            {

                auto conscell = parser_.s_exp_parse();
                while (nullptr != conscell) {
                    std::unique_ptr<syntax::cons_node> cons(
                        static_cast<syntax::cons_node*>(
                            conscell.release()));

                    analyse_cons(std::move(cons));
                    conscell = parser_.s_exp_parse();
                }
            }

            auto analyse_cons(std::unique_ptr<syntax::cons_node>&& cons)
                -> std::unique_ptr<syntax::cons_node>
//                 -> std::unique_ptr<symbol_table_entry>
            {
                // node type check
                assert(syntax::node_type::symbol == cons->car->type);

                auto function_name =
                    static_cast<syntax::string_node*>(cons->car.get())->val;
                //             std::cout << "func: " << function_name << std::endl;

                auto& pair = search_function(function_name);

                // check function definition
                assert(entry_type::function ==
                       static_cast<function_entry*>(pair.second.get())->type_);

                auto arg_num =
                    static_cast<function_entry*>(pair.second.get())->argument_num_;


    //             std::cout << "check cons func_name: "<< function_name << std::endl;
                // argument check of number
                assert(get_argument_number(cons.get()) == arg_num);

                switch (divide_function(function_name)) {
                    case function_type::DEFINE:
                        return define_procedure(std::move(cons->cdr));

                    case function_type::LAMBDA:
                        return lambda_procedure(std::move(cons->cdr));

                    case function_type::OTHER:
                        // TODO what should I do here.
                        return nullptr;
                }

            }


            auto search_function(std::string func_name)
                -> std::pair<std::string, std::unique_ptr<symbol_table_entry>>&
            {
                for(auto& data: symbol_stack.table_stack){
                    if(data.first == func_name){
                        return data;
                    }
                }

                return not_found_pair;
            }

        private:
            syntax::s_expression_parser<Iteratable> parser_;

            struct symbol_table_stack
            {
            public:
                symbol_table_stack() : table_stack(), local_symbol_num(0) {};

            public:
                std::list<
                    std::pair<std::string, std::unique_ptr<symbol_table_entry>>>
                    table_stack;
                int local_symbol_num;
            } symbol_stack;

            std::vector<std::unique_ptr<syntax::cons_node>> function_table;

    };

    template<typename T>
    std::pair<std::string, std::unique_ptr<symbol_table_entry>>
        semantic_analyser<T>::not_found_pair = std::make_pair("",
            std::unique_ptr<symbol_table_entry>(new symbol_table_entry()));

}  // namespace semantic
}  // namespace popo
