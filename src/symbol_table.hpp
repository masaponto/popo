#pragma once

#include <iostream>
#include <utility>
#include <string>

#include "node.hpp"

namespace popo {
namespace semantic {

    enum struct entry_type { value, list, function, not_found };

    enum struct function_type {
        // special form
        sf_define = -1,
        sf_lambda = -2,
        sf_quote = -3,
        sf_if = -4,

        // built in function
        plus = -5,
        minus = -6,
        multiply = -7,
        divide = -8,

        // basic function
        atom = -9,
        eq = -10,
        car = -11,
        cdr = -12,
        cons = 13,

        // other
        other = -99,
    };

    struct symbol_table_entry {

        public:
            symbol_table_entry(entry_type t) : type(t) {};
            symbol_table_entry() : type(entry_type::not_found) {};
            virtual ~symbol_table_entry() {};

        public:
            entry_type type;
    };

    struct function_entry : public symbol_table_entry {

        public:
            function_entry(int arg_num)
                : symbol_table_entry(entry_type::function),
                  argument_num(arg_num),
                  cons(nullptr) {};

//             function_entry(int arg_num)
//                 : function_entry(arg_num, static_cast<int>(num)){};

            function_entry(std::unique_ptr<syntax::expr_node> c)
                : symbol_table_entry(entry_type::function) ,
                argument_num(0), cons(std::move(c)) {};

            function_entry(function_entry&& entry)
                : symbol_table_entry(entry_type::function),
                argument_num(entry.argument_num), cons(std::move(entry.cons)) {};



        public:
            int argument_num;
//             int function_num;
            std::unique_ptr<syntax::expr_node> cons;
    };

    struct value_entry : public symbol_table_entry
    {

        public:
            value_entry(std::string n, syntax::node_type t)
                : symbol_table_entry(entry_type::value), type(t)
            {
                if (t == syntax::node_type::string) {
                    v_string = n;
                } else if (t == syntax::node_type::symbol) {
                    v_symbol = n;
                }
            };

            value_entry(int n, syntax::node_type t)
                : symbol_table_entry(entry_type::value), type(t)
            {
                v_int = n;
            };

//             virtual ~value_entry() {};

        private:
            int v_int;
            std::string v_string;
            std::string v_symbol;
            syntax::node_type type;
    };

    struct list_entry : public symbol_table_entry
    {
        public:
            list_entry(std::unique_ptr<syntax::cons_node>&& list)
                : symbol_table_entry(entry_type::list), list(std::move(list))
            {
            }

        private:
            std::unique_ptr<syntax::cons_node> list;
    };

    const std::pair<std::string, function_entry> special_form[] = {
        std::pair<std::string, function_entry>(
            "define",
            function_entry(2)),
        // (define symbol atom | list | function)
        std::pair<std::string, function_entry>(
            "lambda",
            function_entry(2)),
        // (lambda list list) -> function
        std::pair<std::string, function_entry>(
            "quote",
            function_entry(1)),
        // (quota list) -> list
        std::pair<std::string, function_entry>(
            "if",
            function_entry(3))
        // (if t | nil | function function)
    };

    const std::pair<std::string, function_entry> built_in_function[] = {
        //
        std::pair<std::string, function_entry>("+", function_entry(2)),
        std::pair<std::string, function_entry>("-", function_entry(2)),
        std::pair<std::string, function_entry>("*", function_entry(2)),
        std::pair<std::string, function_entry>("/", function_entry(2)),
        // basic function
        std::pair<std::string, function_entry>("atom", function_entry(2)),
        std::pair<std::string, function_entry>("eq", function_entry(2)),
        std::pair<std::string, function_entry>("car", function_entry(2)),
        std::pair<std::string, function_entry>("cdr", function_entry(2)),
        std::pair<std::string, function_entry>("cons", function_entry(2)),
        // stdio
        std::pair<std::string, function_entry>("read", function_entry(0)),
        std::pair<std::string, function_entry>("write", function_entry(1)), };

} // namespace semantic
} // namespace popo
