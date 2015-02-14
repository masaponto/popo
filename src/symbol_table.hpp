#pragma once

#include <iostream>
#include <utility>
#include <string>

#include "node.hpp"

namespace popo {
namespace semantic {

enum struct entry_type { value, list, closure, not_found };

enum struct function_type {
    // special form
    sf_define,
    sf_lambda,
    sf_quote,
    sf_if,

    // built in function
//     plus = -5,
//     minus = -6,
//     multiply = -7,
//     divide = -8,

    // basic function
//     atom = -9,
//     eq = -10,
//     car = -11,
//     cdr = -12,
//     cons = 13,

    // other
    other,
};

struct symbol_table_entry {

public:
    symbol_table_entry(entry_type t) : type(t) {};
    symbol_table_entry() : type(entry_type::not_found) {};
    virtual ~symbol_table_entry() {};

public:
    entry_type type;
};

struct closure_entry : public symbol_table_entry {
public:
    closure_entry(int argc, std::string l)
        : symbol_table_entry(entry_type::closure), arg_count(argc), label(l) {};

public:
    int arg_count;
    std::string label;
};

struct value_entry : public symbol_table_entry {
public:
    value_entry(int val)
        : symbol_table_entry(entry_type::value),
          int_value(val),
          string_value(),
          node_type(syntax::node_type::num) {};

    value_entry(std::string str, syntax::node_type type)
        : symbol_table_entry(entry_type::value),
          string_value(str),
          node_type(type) {};

public:
    int int_value;
    std::string string_value;
    syntax::node_type node_type;

};
//TODO add another entry

// struct function_entry : public symbol_table_entry {
// public:
//     function_entry(int arg_num)
//         : symbol_table_entry(entry_type::function), argument_num(arg_num) {};

//     function_entry(std::unique_ptr<syntax::cons_node> arg_cons,
//                    std::unique_ptr<syntax::cons_node> func_cons)
//         : symbol_table_entry(entry_type::function),
//           arg(std::move(arg_cons)),
//           func(std::move(func_cons)),
//           argument_num(get_argument_num(arg.get())) {};

// public:
//     std::unique_ptr<syntax::cons_node> arg;
//     std::unique_ptr<syntax::cons_node> func;
//     int argument_num;

// private:
//     auto get_argument_num(const syntax::cons_node* cons) -> int
//     {

//         if (syntax::node_type::nil == cons->type) {
//             return 0;
//         }
//         return get_argument_num(
//                    static_cast<const syntax::cons_node*>(cons->cdr.get())) +
//                1;
//     }
// };

// struct value_entry : public symbol_table_entry {

// public:
//     value_entry(std::string n, syntax::node_type t)
//         : symbol_table_entry(entry_type::value), type(t)
//     {
//         if (t == syntax::node_type::string) {
//             v_string = n;
//         } else if (t == syntax::node_type::symbol) {
//             v_symbol = n;
//         }
//     };

//     value_entry(int n, syntax::node_type t)
//         : symbol_table_entry(entry_type::value), type(t)
//     {
//         v_int = n;
//     };

// public:
//     syntax::node_type type;
//     int v_int;
//     std::string v_string;
//     std::string v_symbol;
// };

// struct list_entry : public symbol_table_entry {
// public:
//     list_entry(std::unique_ptr<syntax::cons_node>&& list)
//         : symbol_table_entry(entry_type::list), list(std::move(list))
//     {}

// private:
//     std::unique_ptr<syntax::cons_node> list;
// };

const std::pair<std::string, int> special_form[] = {
//     std::pair<std::string, >("define", 2),
    std::make_pair("define", 2),
    // (define symbol atom | list | function)
//     std::pair<std::string, int>("lambda", 2),
    std::make_pair("lambda", 2),
//     (lambda list list) -> function
    std::make_pair("quote", 1),
//     std::pair<std::string, int>("quote", 1),
    // (quota list) -> list
    std::make_pair("if", 3),
//     std::pair<std::string, int>("if", 3)
    // (if t | nil | function function)
};

const std::pair<std::string, int> built_in_function[] = {
    //
    std::make_pair("+", 2),
    std::make_pair("-", 2),
    std::make_pair("*", 2),
    std::make_pair("/", 2),
//     std::pair<std::string, int>("+", 2),
//     std::pair<std::string, int>("-", 2),
//     std::pair<std::string, int>("*", 2),
//     std::pair<std::string, int>("/", 2),

    // basic function
    std::make_pair("atom", 2),
    std::make_pair("eq", 2),
    std::make_pair("car", 2),
    std::make_pair("cdr", 2),
    std::make_pair("cons", 2),
//     std::pair<std::string, int>("atom", 2),
//     std::pair<std::string, int>("eq", 2),
//     std::pair<std::string, int>("car", 2),
//     std::pair<std::string, int>("cdr", 2),
//     std::pair<std::string, int>("cons", 2),

    // stdio
    std::make_pair("read", 0),
    std::make_pair("write", 1),
//     std::pair<std::string, int>("read", 0),
//     std::pair<std::string, int>("write", 1),

    // other build in function
    std::make_pair("=", 2),
//     std::pair<std::string, int>("=", 2),
};

} // namespace semantic
} // namespace popo
