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
    enum struct entry_type { value, function, };
    struct symbol_table_entry {

        public:
            symbol_table_entry(entry_type t) : type_(t){};

        public:
            entry_type type_;
    };

    struct function_entry : public symbol_table_entry {

        public:
            function_entry(int arg_num, int entry_num)
                : symbol_table_entry(entry_type::function),
                  argument_num_(arg_num),
                  table_entry_num_(entry_num) {};

            function_entry(int arg_num, function_numbers num):
                function_entry(arg_num, static_cast<int>(num)){};

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
            for (auto&& sf : special_form) {
                symbol_table.table_stack.push_front(sf);
            }

            for (auto&& bf : built_in_function) {
                symbol_table.table_stack.push_front(bf);
            }

            analyse();
        };

    private:
        auto analyse() -> void
        {
            std::unique_ptr<syntax::cons_node> s_exp_node = parser_.s_exp_parse();
//             std::unique_ptr<string_node> function_name(
//                     static_cast<string_node*>(s_exp_node->car.release));
//
//
//             auto function_name = s_exp_node->car->val;
            auto function_name = dynamic_cast<syntax::string_node*>(s_exp_node.get()).val;



            //TODO: このあとのしょり
        }

    private:
        syntax::s_expression_parser<Iteratable> parser_;

        struct symbol_table_stack
        {
        public:
            symbol_table_stack() : table_stack(), local_symbol_num(0) {};

        public:
            std::list<std::pair<std::string, symbol_table_entry> > table_stack;
            int local_symbol_num;
        } symbol_table;

        std::vector<std::unique_ptr<syntax::expr_node> > function_table;
    };


}  // namespace semantic
}  // namespace popo
