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
                for (auto& pair : special_form) {
                    symbol_stack.table_stack.push_front(
                        std::make_pair(pair.first,
                                           std::make_shared<function_entry>(pair.second)));
                }

                for (auto& pair : built_in_function) {
                    symbol_stack.table_stack.push_front(
                        std::make_pair(pair.first,
                                           std::make_shared<function_entry>(pair.second)));
                }

                analyse();
            };

        public:
            static std::pair<std::string, std::shared_ptr<symbol_table_entry>> not_found_pair;
            static std::shared_ptr<symbol_table_entry> dummy_entry;

        private:
            enum struct function_type {
                DEFINE,
                LAMBDA,
                OTHER,
            };

        private:

            auto get_car_depth(syntax::cons_node* cons)
                -> int
            {
                assert(nullptr != cons);
                auto cdr = static_cast<syntax::cons_node*>(cons->cdr.get());
                if(syntax::node_type::nil == cdr->type){
                    return 0;
                }
                else {
                    return get_car_depth(cdr) + 1;
                }
            }

            template<typename n_type>
            auto regist_value_entry(std::string symbol,
                    std::unique_ptr<syntax::cons_node>&& value_node)
                -> std::shared_ptr<symbol_table_entry>&&
            {
                auto node = std::unique_ptr<n_type>(
                    static_cast<n_type*>(value_node->car.release()));

                //                 auto entry = std::unique_ptr<value_entry>(
                //                     new value_entry(node->val, node->type));
                std::shared_ptr<symbol_table_entry> entry =
                    std::make_shared<value_entry>(node->val, node->type);

                symbol_stack.table_stack.push_front(
                    //                     std::make_pair(symbol,
                    // std::move(entry)));
                    std::make_pair(symbol, entry));

                return std::move(entry);
            }

            auto define_procedure(std::unique_ptr<syntax::expr_node>&& cons)
                -> std::shared_ptr<symbol_table_entry>&&
            {
                // TODO: this function is push to table_stack and regist to
                // symbol_table.

                auto cons_node = std::unique_ptr<syntax::cons_node>(
                    static_cast<syntax::cons_node*>(cons.release()));

                // check symbol
                assert(syntax::node_type::symbol == cons_node->car->type);
                auto symbol =
                    std::unique_ptr<syntax::symbol_node>(
                        static_cast<syntax::symbol_node*>(
                            cons_node->car.release()))
                        ->val;


                std::cout << "define: " << symbol << std::endl;
                auto value_cons_node = std::unique_ptr<syntax::cons_node>(
                    static_cast<syntax::cons_node*>(cons_node->cdr.release()));

//                 return regist_value_from_cons(symbol, std::move(value_cons_node));
                switch (value_cons_node->car->type) {
                    case syntax::node_type::cons:
                    {

                        auto&& symbol_ptr =
                            analyse_cons(std::unique_ptr<syntax::cons_node>(
                                static_cast<syntax::cons_node*>(
                                    value_cons_node->car.release())));
                        symbol_stack.table_stack.push_front(
                                std::make_pair(symbol, symbol_ptr));

                        //TODO: if symbol_ptr is function_entry, add function_table
                        //                         auto entry =
                        // std::unique_ptr<function_entry>(
                        //                             new
                        // function_entry(get_argument_number(
                        //                                 sym.get()),
                        // function_table.size()));
                        //                         function_table.push_back(entry);
                        return std::move(symbol_ptr);
                    }
                    case syntax::node_type::num:
                        return regist_value_entry<syntax::num_node>(
                            symbol, std::move(value_cons_node));

                    case syntax::node_type::string:
                        return regist_value_entry<syntax::string_node>(
                            symbol, std::move(value_cons_node));

                    case syntax::node_type::symbol:
                        return regist_value_entry<syntax::symbol_node>(
                            symbol, std::move(value_cons_node));

                    case syntax::node_type::nil: {
                        std::shared_ptr<symbol_table_entry> entry =
                            std::make_shared<list_entry>(
                                std::unique_ptr<syntax::cons_node>(
                                    new syntax::cons_node()));

                        symbol_stack.table_stack.push_front(
                            std::make_pair(symbol, entry));

                        return std::move(entry);
                    }
                }

            }

            auto push_arguments(const syntax::cons_node* cons)
                -> int
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

                return push_arguments(static_cast<const syntax::cons_node*>(
                           cons->cdr.get())) + 1;
            }

            auto lambda_procedure(std::unique_ptr<syntax::expr_node>&& cons)
                -> std::shared_ptr<symbol_table_entry>&&
            {
                std::cout << "lambda" << std::endl;
                auto cons_node = std::unique_ptr<syntax::cons_node>(
                    static_cast<syntax::cons_node*>(cons.release()));

                auto function_cons_tmp =
                    std::move(std::unique_ptr<syntax::cons_node>(
                                  static_cast<syntax::cons_node*>(
                                      cons_node->cdr.release()))->car);

                auto argument_cons = std::unique_ptr<syntax::cons_node>(
                    static_cast<syntax::cons_node*>(
                        std::move(cons_node->car).release()));

                //push dummy argument to symbol stack
                auto argument_count = push_arguments(argument_cons.get());

                auto function_cons = std::unique_ptr<syntax::cons_node>(
                    static_cast<syntax::cons_node*>(
                        function_cons_tmp.release()));

                // create symbol entry
                std::shared_ptr<symbol_table_entry> entry =
                    std::make_shared<function_entry>(argument_count,
                                                     function_table.size());

                auto& f_cons = function_cons;
                assert(dummy_entry == analyse_cons(std::move(f_cons)));

                // set function table
                function_table.push_back(std::move(function_cons));
                //TODO: check function procedure
                // pop dummy argumeny at symbol stack
                for(int i=0; i<symbol_stack.local_symbol_num; ++i){
                    symbol_stack.table_stack.pop_front();
                }

                return std::move(entry);
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
                -> std::shared_ptr<symbol_table_entry>&&
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
                assert(get_car_depth(cons.get()) == arg_num);

                switch (divide_function(function_name)) {
                    case function_type::DEFINE:
                        return define_procedure(std::move(cons->cdr));

                    case function_type::LAMBDA:
                        return lambda_procedure(std::move(cons->cdr));

                    case function_type::OTHER:
                        auto dummy = dummy_entry;
                        return std::move(dummy);
                }

            }


            auto search_function(std::string func_name)
                -> std::pair<std::string, std::shared_ptr<symbol_table_entry>>&
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
                    std::pair<std::string, std::shared_ptr<symbol_table_entry>>>
                    table_stack;
                int local_symbol_num;
            } symbol_stack;

            std::vector<std::unique_ptr<syntax::cons_node>> function_table;

    };

    template<typename T>
    std::pair<std::string, std::shared_ptr<symbol_table_entry>>
        semantic_analyser<T>::not_found_pair = std::make_pair("",
            std::make_shared<symbol_table_entry>());

    template<typename T>
    std::shared_ptr<symbol_table_entry> semantic_analyser<T>::dummy_entry =
        std::make_shared<symbol_table_entry>();

}  // namespace semantic
}  // namespace popo
