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

        // other
        other = -9,
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
                  argument_num(arg_num),
                  function_num(entry_num), cons() {};

            function_entry(int arg_num, function_type num)
                : function_entry(arg_num, static_cast<int>(num)){};

            function_entry(int arg_num, std::unique_ptr<syntax::expr_node> c)
                : symbol_table_entry(entry_type::function) ,
                argument_num(arg_num), function_num(0), cons(std::move(c)) {};

            function_entry(function_entry&& obj)
                : symbol_table_entry(entry_type::function),
                argument_num(obj.argument_num), function_num(obj.function_num),
                cons(std::move(obj.cons))
        {};


        public:
            int argument_num;
            int function_num;
            std::unique_ptr<syntax::expr_node> cons;
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
            function_entry(2, function_type::sf_define)),
        // (define symbol atom | list | function)
        std::pair<std::string, function_entry>(
            "lambda",
            function_entry(2, function_type::sf_lambda)),
        // (lambda list list) -> function
        std::pair<std::string, function_entry>(
            "quote",
            function_entry(1, function_type::sf_quote)),
        // (quota list) -> list
        std::pair<std::string, function_entry>(
            "if",
            function_entry(3, function_type::sf_if))
        // (if t | nil | function function)
    };

    const std::pair<std::string, function_entry> built_in_function[] = {
        std::pair<std::string, function_entry>(
            "+",
            function_entry(2, function_type::plus)),
        std::pair<std::string, function_entry>(
            "-",
            function_entry(2, function_type::minus)),
        std::pair<std::string, function_entry>(
            "*",
            function_entry(2, function_type::multiply)),
        std::pair<std::string, function_entry>(
            "/",
            function_entry(2, function_type::divide))};

} // namespace semantic
} // namespace popo
