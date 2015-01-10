#pragma once

#include <iostream>
#include <utility>
#include <string>

#include "node.hpp"

namespace popo {
namespace semantic {

    enum struct entry_type { value, list, function, not_found };

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


    struct symbol_table_entry {

        public:
            symbol_table_entry(entry_type t) : type_(t) {};
            symbol_table_entry() : type_(entry_type::not_found) {};
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

    struct value_entry : public symbol_table_entry
    {

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

//             virtual ~value_entry() {};

        private:
            int v_int;
            std::string v_string;
            std::string v_symbol;
            syntax::node_type type_;
    };

    struct list_entry : public symbol_table_entry
    {
        public:
            list_entry(std::unique_ptr<syntax::cons_node>&& list)
                : symbol_table_entry(entry_type::list), list_(std::move(list))
            {
            }

        private:
            std::unique_ptr<syntax::cons_node> list_;
    };

    const std::pair<std::string, function_entry> special_form[] = {
        std::pair<std::string, function_entry>(
            "define",
            function_entry(2, function_numbers::DEFINE)),
        // (define symbol atom | list | function)
        std::pair<std::string, function_entry>(
            "lambda",
            function_entry(2, function_numbers::LAMBDA)),
        // (lambda list list) -> function
        std::pair<std::string, function_entry>(
            "quota",
            function_entry(1, function_numbers::QUOTA)),
        // (quota list) -> list
        std::pair<std::string, function_entry>(
            "if",
            function_entry(2, function_numbers::IF))
        // (if t | nil | function function)
    };

    const std::pair<std::string, function_entry> built_in_function[] = {
        std::pair<std::string, function_entry>(
            "+",
            function_entry(2, function_numbers::PLUS)),
        std::pair<std::string, function_entry>(
            "-",
            function_entry(2, function_numbers::MINUS)),
        std::pair<std::string, function_entry>(
            "*",
            function_entry(2, function_numbers::MULTIPLY)),
        std::pair<std::string, function_entry>(
            "/",
            function_entry(2, function_numbers::DIVIDE))};

} // namespace semantic
} // namespace popo