#pragma once

#include <string>
#include <cassert>
#include <vector>
#include <list>
#include <utility>
#include "syntax.hpp"


namespace popo {
namespace semantic {

    enum struct function_numbers{
        //special form
        DEFINE = -1,
        LAMBDA = -2,
        QUOTA = -3,
        IF = -4,

        //built in function
        PLUS = -5,
        MINUS = -6,
        MULTIPLY = -7,
        DIVIDE = -8,
    };

    // symbol table
    enum struct entry_type { value, function, not_found};
    struct symbol_table_entry {

        public:
            symbol_table_entry(entry_type t) : type_(t){};
            symbol_table_entry() : type_(entry_type::not_found){};
            virtual ~symbol_table_entry() {};

        public:
            entry_type type_;
    };

    struct function_entry : public symbol_table_entry {

        public:
            function_entry(int arg_num, int entry_num)
                : symbol_table_entry(entry_type::function),
                  argument_num_(arg_num),
                  table_entry_num_(entry_num) {};

            function_entry(int arg_num, function_numbers num)
                : function_entry(arg_num, static_cast<int>(num)){};

        public:
            int argument_num_;
            int table_entry_num_;
    };

    struct value_entry : public symbol_table_entry {

    public:

    public:
        value_entry(std::string n, syntax::node_type t)
            : symbol_table_entry(entry_type::value), type_(t)
        {
            if (t == syntax::node_type::string) {
                v_string = n;
            } else if (t == syntax::node_type::symbol) {
                v_symbol = n;
            }
        };

        value_entry(int n, syntax::node_type t)
            : symbol_table_entry(entry_type::value), type_(t)
        {
            v_int = n;
        };

        virtual ~value_entry() {};

    private:
        int v_int;
        std::string v_string;
        std::string v_symbol;
        syntax::node_type type_;
    };

    const std::pair<std::string, function_entry> special_form[] = {
        std::pair<std::string, function_entry>(
            "define", function_entry(2, function_numbers::DEFINE)),
        // (define symbol atom | list | function)
        std::pair<std::string, function_entry>(
            "lambda", function_entry(2, function_numbers::LAMBDA)),
        // (lambda list list) -> function
        std::pair<std::string, function_entry>(
            "quota", function_entry(1, function_numbers::QUOTA)),
        // (quota list) -> list
        std::pair<std::string, function_entry>(
            "if", function_entry(2, function_numbers::IF))
        // (if t | nil | function function)
    };

    const std::pair<std::string, function_entry> built_in_function[] = {
        std::pair<std::string ,function_entry>(
                "+", function_entry(2, function_numbers::PLUS)),

        std::pair<std::string ,function_entry>(
                "-", function_entry(2, function_numbers::MINUS)),

        std::pair<std::string ,function_entry>(
                "*", function_entry(2, function_numbers::MULTIPLY)),

        std::pair<std::string ,function_entry>(
                "/", function_entry(2, function_numbers::DIVIDE))
    };

    template <typename Iteratable>
    class semantic_analyser {

    public:
        semantic_analyser(const Iteratable& itr)
            : parser_(itr), symbol_table(), function_table()
        {
            for (auto&& pair: special_form) {
                symbol_table.table_stack.push_front(
                    std::make_pair(
                        pair.first,
                        std::unique_ptr<symbol_table_entry>(new function_entry(pair.second))));
            }

            for (auto&& pair : built_in_function) {
                symbol_table.table_stack.push_front(
                    std::make_pair(
                        pair.first,
                        std::unique_ptr<symbol_table_entry>(new function_entry(pair.second))));
            }

            analyse();
            print_symbol_stack();
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

        auto check_argument_num(const syntax::expr_node* cons, int argc)
            -> bool
        {
            if (syntax::node_type::nil == cons->type) {

                if (argc == -1) {
                    return true;
                } else {
                    return false;
                }
            } else if (argc < -1) {
                return false;
            }

            return check_argument_num(
                static_cast<const syntax::cons_node*>(cons)->cdr.get(), --argc);
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

            symbol_table.table_stack.push_front(
                std::make_pair(symbol, std::move(entry)));
        }

        auto define_procedure(std::unique_ptr<syntax::expr_node>&& cons)
            -> void
        {
            // TODO: this function is push to table_stack and regist to
            // function_table.
            auto cons_node = std::unique_ptr<syntax::cons_node>(
                static_cast<syntax::cons_node*>(cons.release()));

            // check symbol
            assert(syntax::node_type::symbol == cons_node->car->type);
            auto symbol =
                std::unique_ptr<syntax::symbol_node>(
                    static_cast<syntax::symbol_node*>(cons_node->car.release()))
                    ->val;

//             std::cout << "define: " << symbol << std::endl;

            auto value_cons_node = std::unique_ptr<syntax::cons_node>(
                static_cast<syntax::cons_node*>(cons_node->cdr.release()));

            switch (value_cons_node->car->type) {
                case syntax::node_type::cons:

                    check_cons(std::unique_ptr<syntax::cons_node>(
                        static_cast<syntax::cons_node*>(
                            value_cons_node->car.release())));

                    //TODO make symbol_table_entry(function_entry).
                    break;

                case syntax::node_type::num:
                    regist_value_entry<syntax::num_node>(symbol, std::move(value_cons_node));
                    break;

                case syntax::node_type::string:
                    regist_value_entry<syntax::string_node>(symbol, std::move(value_cons_node));
                    break;

                case syntax::node_type::symbol:
                    regist_value_entry<syntax::symbol_node>(symbol, std::move(value_cons_node));
                    break;

                case syntax::node_type::nil: {
                    //TODO write code
                    return;
                }
            }

        }

        auto lambda_procedure(std::unique_ptr<syntax::expr_node>&& cons)
            -> void
        {
//             std::cout << "lambda" <<  std::endl;
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

                check_cons(std::move(cons));
                conscell = parser_.s_exp_parse();
            }
        }

        auto check_cons(std::unique_ptr<syntax::cons_node>&& cons)
            -> void

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

            // argument check of number
            assert(check_argument_num(cons.get(), arg_num));

            switch (divide_function(function_name)) {
                case function_type::DEFINE:
                    define_procedure(std::move(cons->cdr));
                    break;

                case function_type::LAMBDA:
                    lambda_procedure(std::move(cons->cdr));
                    break;

                case function_type::OTHER:

                    break;
            }
        }


        auto search_function(std::string func_name)
            -> std::pair<std::string, std::unique_ptr<symbol_table_entry>>&
        {
            for(auto& data: symbol_table.table_stack){
                if(data.first == func_name){
                    return data;
                }
            }

            return not_found_pair;
        }

        auto print_symbol_stack() -> void
        {
            for(auto& data: symbol_table.table_stack){
                std::cout << "symbol name: " << data.first << std::endl;
            }
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
        } symbol_table;

        // TODO: rename symbol_table or function_table?
        std::vector<std::unique_ptr<syntax::cons_node>> function_table;


    };

    template<typename T>
    std::pair<std::string, std::unique_ptr<symbol_table_entry>>
        semantic_analyser<T>::not_found_pair = std::make_pair("",
            std::unique_ptr<symbol_table_entry>(new symbol_table_entry()));


}  // namespace semantic
}  // namespace popo
