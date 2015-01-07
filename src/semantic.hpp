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
            enum struct value_type { INT, STRING, SYMBOL, };

        public:
            value_entry(std::string n, value_type t)
                : symbol_table_entry(entry_type::value), type_(t)
            {
                if (t == value_type::STRING) {
                    v_string = n;
                } else if (t == value_type::SYMBOL) {
                    v_symbol = n;
                }
            };

            value_entry(int n, value_type t)
                : symbol_table_entry(entry_type::value), type_(t)
            {
                v_int = n;
            };

            virtual ~value_entry() {};

        private:
            union {
                int v_int;
                std::string v_string;
                std::string v_symbol;
            };
            value_type type_;
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
        };

    public:
        static std::pair<std::string, std::unique_ptr<symbol_table_entry>> not_found_pair;

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

        auto analyse() -> void
        {

            std::unique_ptr<syntax::cons_node> cons(
                static_cast<syntax::cons_node*>(
                    parser_.s_exp_parse().release()));

            // type check
            assert(syntax::node_type::string == cons->car->type);

            auto function_name =
                static_cast<syntax::string_node*>(cons->car.get())->val;

            auto& pair = search_function(function_name);
            // check function definition
            assert(entry_type::function ==
                   static_cast<function_entry*>(pair.second.get())->type_);

            // TODO しらべる below code
            auto arg_num =
                static_cast<function_entry*>(pair.second.get())->argument_num_;
            std::cout << "arg: " << arg_num << std::endl;
            // argument check of number
            assert(check_argument_num(cons.get(), arg_num));
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


    private:
        syntax::s_expression_parser<Iteratable> parser_;

        struct symbol_table_stack
        {
            public:
                symbol_table_stack() : table_stack(), local_symbol_num(0) {};

            public:
                std::list<std::pair<std::string, std::unique_ptr<symbol_table_entry>>> table_stack;
                int local_symbol_num;
        } symbol_table;

        std::vector<std::unique_ptr<syntax::cons_node>> function_table;
    };

    template<typename T>
    std::pair<std::string, std::unique_ptr<symbol_table_entry>>
        semantic_analyser<T>::not_found_pair = std::make_pair("",
            std::unique_ptr<symbol_table_entry>(new symbol_table_entry()));


}  // namespace semantic
}  // namespace popo
