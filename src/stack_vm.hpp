#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <stack>
#include <cassert>
#include <map>
#include <memory>
#include <regex>

#include "vm_symbol.hpp"

namespace popo {

    namespace stack_vm {

    class vm {
    public:
        vm() {};
        vm(std::string ir_code) : ir_code_ss(ir_code) {}
        ~vm() {}

    private:
        std::stringstream ir_code_ss;
        std::list<std::shared_ptr<instruction>> instruction_list;
        std::stack<std::shared_ptr<element>> stack;
        std::map<std::string, std::shared_ptr<function>> function_table;
        std::list<std::map<std::string, std::shared_ptr<symbol_entry>>>
            symbol_table_list;

    public:
        auto parse() -> void
        {
            std::string s;
            std::list<std::string> ir_list;
            std::vector<std::string> inst_vec;

            while (std::getline(ir_code_ss, s, '\n')) {
                ir_list.push_back(s);
            }

            for (auto it = ir_list.begin(); it != ir_list.end(); ++it) {
                std::stringstream inst_ss(*it);

                while (std::getline(inst_ss, s, ' ')) {
                    inst_vec.push_back(s);
                }

                instruction_list.push_back(create_inst(inst_vec));
                inst_vec.clear();
            }

            run();
        }

        auto parse(std::list<std::string> i_list) -> void
        {
            std::string tmp;
            std::vector<std::string> inst_vec;

            std::cout << "--- ir ---" << std::endl;;
            for (auto line : i_list) {
                std::cout << line << std::endl;

                std::stringstream inst_ss(line);
                while (std::getline(inst_ss, tmp, ' ')) {
                    inst_vec.push_back(tmp);
                }
                instruction_list.push_back(create_inst(inst_vec));
                inst_vec.clear();
            }
            run();
        }

    public:
        auto run() -> void
        {
            std::map<std::string, std::shared_ptr<symbol_entry>> global_sym;
            symbol_table_list.push_back(global_sym);

            for (auto it = instruction_list.begin();
                 it != instruction_list.end();
                 ++it) {

                switch ((*it)->op) {
                    case operation::main: {
                        // std::cout << "main function" << std::endl;

                        break;
                    }

                    case operation::func: {
                        // std::cout << "function define" << std::endl;
                        auto op_ins =
                            std::static_pointer_cast<op_instruction>(*it);
                        auto func_ele =
                            std::static_pointer_cast<symbol_element>(
                                op_ins->operand);

                        const std::string func_name = func_ele->data;

                        std::list<std::shared_ptr<instruction>> func_code;
                        int arg_num = 0;
                        // int ret_num = 1;

                        while ((*it)->op != operation::ret) {

                            if ((*it)->op == operation::param) {
                                arg_num++;
                            }

                            func_code.push_back(std::move(*it));
                            ++it;
                        }

                        function_table.insert(make_pair(
                            func_name,
                            std::shared_ptr<function>(new function(
                                func_name, arg_num, std::move(func_code)))));

                        // std::cout << func_name << std::endl;
                        break;
                    }

                    default: {
                        symbol_table_list =
                            stack_manager(std::move(*it), symbol_table_list);
                    }
                }
            }
        }

    public:
        auto stack_manager(
            std::shared_ptr<instruction> ins,
            std::list<std::map<std::string, std::shared_ptr<symbol_entry>>>
                sym_tables)
            -> std::list<std::map<std::string, std::shared_ptr<symbol_entry>>>
        {
            auto sym = sym_tables.begin();

            switch (ins->op) {
                case operation::write: {
                    if (!stack.empty()) {
                        auto e = stack.top();

                        switch (e->type) {
                            case element_type::integer: {
                                auto e_int =
                                    std::static_pointer_cast<int_element>(e);

                                std::cout << e_int->data << std::endl;
                                break;
                            }
                            case element_type::real: {
                                auto e_real =
                                    std::static_pointer_cast<real_element>(e);
                                std::cout << e_real->data << std::endl;
                                break;
                            }
                            case element_type::boolean: {
                                auto e_bool =
                                    std::static_pointer_cast<bool_element>(e);
                                auto btos = [](bool b) {
                                    return b ? "true" : "false";
                                };
                                std::cout << btos(e_bool->data) << std::endl;
                                break;
                            }
                            case element_type::real_list: {
                                auto e_list =
                                    std::static_pointer_cast<real_list_element>(
                                        e);
                                std::cout << "(";
                                for (auto it = e_list->data.begin();
                                     it != e_list->data.end();
                                     ++it) {
                                    std::cout << *it << " ";
                                }
                                std::cout << ")" << std::endl;
                                break;
                            }
                            case element_type::int_list: {
                                auto e_list =
                                    std::static_pointer_cast<int_list_element>(
                                        e);
                                std::cout << "(";
                                for (auto it = e_list->data.begin();
                                     it != e_list->data.end();
                                     ++it) {
                                    std::cout << *it << " ";
                                }
                                std::cout << ")" << std::endl;
                                break;
                            }
                            default:
                                std::cout << "not implemented" << std::endl;
                        }

                    } else {
                        std::cout << "Ooops, stack is empty :(" << std::endl;
                    }
                    break;
                }

                case operation::push_int:
                case operation::push_float:
                // case operation::push_string :
                case operation::push_bool:
                    // case operation::push_list :
                    {
                        auto op_ins =
                            std::static_pointer_cast<op_instruction>(ins);
                        stack.push(std::move(op_ins->operand));
                        break;
                    }
                case operation::push_symbol: {

                    auto op_ins = std::static_pointer_cast<op_instruction>(ins);

                    if (op_ins->operand->type == element_type::symbol) {

                        auto el = std::static_pointer_cast<symbol_element>(
                            op_ins->operand);
                        bool find_flag = false;

                        for (auto sym_table : sym_tables) {

                            auto sym_it = sym_table.find(el->data);

                            if (sym_it != sym_table.end()) {
                                find_flag = true;
                                if (sym_it->second->sclass == sym_class::var) {
                                    auto v =
                                        std::static_pointer_cast<var_entry>(
                                            sym_it->second);
                                    stack.push(v->data);
                                    // element new_e = *(v->data);
                                    // auto ep =
                                    // std::make_shared<element>(new_e);
                                    // stack.push(std::move(ep));
                                } else {
                                    stack.push(std::move(el));
                                }
                                break;
                            }
                        }

                        if (!find_flag) {
                            stack.push(std::move(el));
                        }

                    } else {
                        stack.push(std::move(op_ins->operand));
                    }

                    break;
                }
                case operation::make_list: {
                    auto op_ins = std::static_pointer_cast<op_instruction>(ins);
                    auto size_e =
                        std::static_pointer_cast<int_element>(op_ins->operand);

                    int size = size_e->data;

                    std::list<std::shared_ptr<element>> e_list;

                    for (int i = 0; i < size; i++) {
                        e_list.push_front(std::move(stack.top()));
                        stack.pop();
                    }

                    auto e = e_list.begin();

                    switch ((*e)->type) {
                        case element_type::integer: {
                            std::list<int> int_list;
                            for (auto it = e_list.begin(); it != e_list.end();
                                 ++it) {
                                auto int_e =
                                    std::static_pointer_cast<int_element>(*it);
                                int_list.push_back(int_e->data);
                            }

                            stack.push(std::shared_ptr<element>(
                                new int_list_element(int_list)));
                            break;
                        }
                        case element_type::real: {
                            std::list<float> real_list;
                            for (auto it = e_list.begin(); it != e_list.end();
                                 ++it) {
                                auto real_e =
                                    std::static_pointer_cast<real_element>(*it);
                                real_list.push_back(real_e->data);
                            }

                            stack.push(std::shared_ptr<element>(
                                new real_list_element(real_list)));
                            break;
                        }
                        default: {
                            std::cout << "not implemented" << std::endl;
                        }
                    }

                    break;
                }
                case operation::branch: {
                    auto op_ins = std::static_pointer_cast<op_instruction>(ins);
                    auto branch_e = std::static_pointer_cast<branch_element>(
                        op_ins->operand);
                    auto e = std::move(stack.top());
                    stack.pop();
                    auto bool_e = std::static_pointer_cast<bool_element>(e);

                    if (bool_e->data) {
                        auto it = function_table.find(branch_e->t_label);
                        run_func(it->second->code);
                        function_table.erase(branch_e->t_label);
                    } else {
                        auto it = function_table.find(branch_e->f_label);
                        run_func(it->second->code);
                        function_table.erase(branch_e->f_label);
                    }

                    break;
                }
                case operation::apply: {
                    auto func_e = std::move(stack.top());
                    stack.pop();

                    switch (func_e->type) {
                        case element_type::add: {
                            auto add_ii = [](int a, int b)
                                              ->int { return a + b; };
                            auto add_fi = [](float a, int b)
                                              ->float { return a + b; };
                            auto add_if = [](int a, float b)
                                              ->float { return a + b; };
                            auto add_ff = [](float a, float b)
                                              ->float { return a + b; };
                            calc(add_ii, add_fi, add_if, add_ff);
                            break;
                        }
                        case element_type::sub: {
                            auto sub_ii = [](int a, int b)
                                              ->int { return a - b; };
                            auto sub_fi = [](float a, int b)
                                              ->float { return a - b; };
                            auto sub_if = [](int a, float b)
                                              ->float { return a - b; };
                            auto sub_ff = [](float a, float b)
                                              ->float { return a - b; };
                            calc(sub_ii, sub_fi, sub_if, sub_ff);
                            break;
                        }
                        case element_type::mul: {
                            auto mul_ii = [](int a, int b)
                                              ->int { return a * b; };
                            auto mul_fi = [](float a, int b)
                                              ->float { return a * b; };
                            auto mul_if = [](int a, float b)
                                              ->float { return a * b; };
                            auto mul_ff = [](float a, float b)
                                              ->float { return a * b; };
                            calc(mul_ii, mul_fi, mul_if, mul_ff);
                            break;
                        }
                        case element_type::div: {
                            auto div_ii = [](int a, int b)
                                              ->int { return a / b; };
                            auto div_fi = [](float a, int b)
                                              ->float { return a / b; };
                            auto div_if = [](int a, float b)
                                              ->float { return a / b; };
                            auto div_ff = [](float a, float b)
                                              ->float { return a / b; };
                            calc(div_ii, div_fi, div_if, div_ff);
                            break;
                        }
                        case element_type::eq: {
                            auto eq_ii = [](int a, int b)
                                             ->int { return a == b; };
                            auto eq_fi = [](float a, int b)
                                             ->float { return a == b; };
                            auto eq_if = [](int a, float b)
                                             ->float { return a == b; };
                            auto eq_ff = [](float a, float b)
                                             ->float { return a == b; };
                            bool_calc(eq_ii, eq_fi, eq_if, eq_ff);
                            break;
                        }
                        case element_type::lt: {
                            auto lt_ii = [](int a, int b)
                                             ->int { return a < b; };
                            auto lt_fi = [](float a, int b)
                                             ->float { return a < b; };
                            auto lt_if = [](int a, float b)
                                             ->float { return a < b; };
                            auto lt_ff = [](float a, float b)
                                             ->float { return a < b; };
                            bool_calc(lt_ii, lt_fi, lt_if, lt_ff);
                            break;
                        }
                        case element_type::mt: {
                            auto mt_ii = [](int a, int b)
                                             ->int { return a > b; };
                            auto mt_fi = [](float a, int b)
                                             ->float { return a > b; };
                            auto mt_if = [](int a, float b)
                                             ->float { return a > b; };
                            auto mt_ff = [](float a, float b)
                                             ->float { return a > b; };
                            bool_calc(mt_ii, mt_fi, mt_if, mt_ff);
                            break;
                        }
                        case element_type::lte: {
                            auto lte_ii = [](int a, int b)
                                              ->int { return a <= b; };
                            auto lte_fi = [](float a, int b)
                                              ->float { return a <= b; };
                            auto lte_if = [](int a, float b)
                                              ->float { return a <= b; };
                            auto lte_ff = [](float a, float b)
                                              ->float { return a <= b; };
                            bool_calc(lte_ii, lte_fi, lte_if, lte_ff);
                            break;
                        }
                        case element_type::mte: {
                            auto mte_ii = [](int a, int b)
                                              ->int { return a >= b; };
                            auto mte_fi = [](float a, int b)
                                              ->float { return a >= b; };
                            auto mte_if = [](int a, float b)
                                              ->float { return a >= b; };
                            auto mte_ff = [](float a, float b)
                                              ->float { return a >= b; };
                            bool_calc(mte_ii, mte_fi, mte_if, mte_ff);
                            break;
                        }

                        case element_type::symbol: {
                            auto symbol_e =
                                std::static_pointer_cast<symbol_element>(
                                    func_e);

                            for (auto sym_table : sym_tables) {

                                auto sym_it = sym_table.find(symbol_e->data);

                                if (sym_it != sym_table.end() &&
                                    sym_it->second->sclass == sym_class::func) {

                                    auto func_e =
                                        std::static_pointer_cast<func_entry>(
                                            sym_it->second);

                                    run_func(func_e->func->code);
                                    break;
                                }
                            }

                            break;
                        }

                        case element_type::define: {
                            auto name_e = std::move(stack.top());
                            stack.pop();
                            auto data_e = std::move(stack.top());
                            stack.pop();

                            auto name_sym_e =
                                std::static_pointer_cast<symbol_element>(
                                    name_e);

                            (*sym) = define(name_sym_e, data_e, *sym);

                            break;
                        }
                        case element_type::cdr: {
                            auto cons_e = std::move(stack.top());
                            stack.pop();

                            switch (cons_e->type) {
                                case element_type::int_list: {
                                    auto int_cons_e = std::static_pointer_cast<
                                        int_list_element>(cons_e);
                                    int_cons_e->data.pop_front();

                                    stack.push(std::move(int_cons_e));
                                    break;
                                }
                                case element_type::real_list: {
                                    auto real_cons_e = std::static_pointer_cast<
                                        real_list_element>(cons_e);
                                    real_cons_e->data.pop_front();

                                    stack.push(std::move(real_cons_e));
                                    break;
                                }
                                default: {
                                    std::cout << "not implemented" << std::endl;
                                }
                            }
                            break;
                        }
                        case element_type::car: {
                            auto cons_e = std::move(stack.top());
                            stack.pop();

                            switch (cons_e->type) {
                                case element_type::int_list: {
                                    auto int_cons_e = std::static_pointer_cast<
                                        int_list_element>(cons_e);
                                    auto i = int_cons_e->data.front();
                                    std::shared_ptr<element> e(
                                        new int_element(i));
                                    stack.push(std::move(e));
                                    break;
                                }
                                case element_type::real_list: {
                                    auto real_cons_e = std::static_pointer_cast<
                                        real_list_element>(cons_e);
                                    auto i = real_cons_e->data.front();
                                    std::shared_ptr<element> e(
                                        new real_element(i));
                                    stack.push(std::move(e));
                                    break;
                                }
                                default: {
                                    std::cout << "car is for list" << std::endl;
                                }
                            }
                            break;
                        }

                        default: {
                            std::cout << "not implemented" << std::endl;
                        } break;
                    }  // end apply case
                    break;
                }
                default: {
                    // std::cout << "not implemented" << std::endl;
                }

            }  // end push_symbol case

            return sym_tables;
        }

        auto define(std::shared_ptr<symbol_element> name_e,
                    std::shared_ptr<element> data_e,
                    std::map<std::string, std::shared_ptr<symbol_entry>> sym)
            -> std::map<std::string, std::shared_ptr<symbol_entry>>
        {
            switch (data_e->type) {

                case element_type::integer: {
                    auto e_int = std::static_pointer_cast<int_element>(data_e);
                    std::shared_ptr<symbol_entry> int_var(
                        new var_entry(name_e->data, std::move(e_int)));
                    sym.insert(make_pair(name_e->data, std::move(int_var)));
                    break;
                }
                case element_type::real: {
                    auto e_real =
                        std::static_pointer_cast<real_element>(data_e);
                    std::shared_ptr<symbol_entry> real_var(
                        new var_entry(name_e->data, std::move(e_real)));
                    sym.insert(make_pair(name_e->data, std::move(real_var)));
                    break;
                }
                case element_type::symbol: {
                    auto e_func =
                        std::static_pointer_cast<symbol_element>(data_e);
                    auto fn_it = function_table.find(e_func->data);

                    if (fn_it != function_table.end()) {
                        std::shared_ptr<symbol_entry> func(
                            new func_entry(name_e->data, fn_it->second));
                        sym.insert(make_pair(name_e->data, std::move(func)));
                    } else {
                        std::cout << "Oooops the function is not defined"
                                  << std::endl;
                    }

                    break;
                }
                case element_type::int_list: {
                    auto e_intl =
                        std::static_pointer_cast<int_list_element>(data_e);
                    std::shared_ptr<symbol_entry> int_list_var(
                        new var_entry(name_e->data, std::move(e_intl)));
                    sym.insert(
                        make_pair(name_e->data, std::move(int_list_var)));

                    break;
                }
                case element_type::real_list: {
                    auto e_reall =
                        std::static_pointer_cast<real_list_element>(data_e);
                    std::shared_ptr<symbol_entry> real_list_var(
                        new var_entry(name_e->data, std::move(e_reall)));
                    sym.insert(
                        make_pair(name_e->data, std::move(real_list_var)));

                    break;
                }
                default: {
                    std::cout << "not implemented" << std::endl;
                }

            }  // end switch

            return sym;
        }

        auto run_func(std::list<std::shared_ptr<instruction>> code) -> void
        {
            std::map<std::string, std::shared_ptr<symbol_entry>> local_sym;
            symbol_table_list.push_front(local_sym);
            auto sym = symbol_table_list.begin();

            for (auto inst_it = code.begin(); inst_it != code.end();
                 ++inst_it) {

                switch ((*inst_it)->op) {
                    case operation::param: {
                        auto op_ins =
                            std::static_pointer_cast<op_instruction>(*inst_it);
                        auto symbol_e =
                            std::static_pointer_cast<symbol_element>(
                                op_ins->operand);

                        auto arg_e = std::move(stack.top());
                        stack.pop();

                        (*sym) = define(symbol_e, arg_e, *sym);

                        break;
                    }
                    default: {
                        // TODO:
                        symbol_table_list.push_front(*sym);
                        symbol_table_list =
                            stack_manager(*inst_it, symbol_table_list);
                        break;
                    }
                }
            }

            symbol_table_list.erase(symbol_table_list.begin());
        }

        template <typename Func1,
                  typename Func2,
                  typename Func3,
                  typename Func4>
        auto calc(Func1 f1, Func2 f2, Func3 f3, Func4 f4) -> void
        {

            auto e2 = std::move(stack.top());
            stack.pop();
            auto e1 = std::move(stack.top());
            stack.pop();

            if (e1->type == element_type::integer &&
                e2->type == element_type::integer) {

                auto e1_int = std::static_pointer_cast<int_element>(e1);
                auto e2_int = std::static_pointer_cast<int_element>(e2);

                std::shared_ptr<element> e3(
                    new int_element(f1(e1_int->data, e2_int->data)));
                stack.push(std::move(e3));

            } else if (e1->type == element_type::real &&
                       e2->type == element_type::integer) {

                auto e1_real = std::static_pointer_cast<real_element>(e1);
                auto e2_int = std::static_pointer_cast<int_element>(e2);

                std::shared_ptr<element> e3(
                    new real_element(f2(e1_real->data, e2_int->data)));
                stack.push(std::move(e3));
            } else if (e1->type == element_type::integer &&
                       e2->type == element_type::real) {

                auto e1_int = std::static_pointer_cast<int_element>(e1);
                auto e2_real = std::static_pointer_cast<real_element>(e2);

                std::shared_ptr<element> e3(
                    new real_element(f3(e1_int->data, e2_real->data)));
                stack.push(std::move(e3));
            } else {

                auto e1_real = std::static_pointer_cast<real_element>(e1);
                auto e2_real = std::static_pointer_cast<real_element>(e2);

                std::shared_ptr<element> e3(
                    new real_element(f4(e1_real->data, e2_real->data)));
                stack.push(std::move(e3));
            }
        }

        template <typename Func1,
                  typename Func2,
                  typename Func3,
                  typename Func4>
        auto bool_calc(Func1 f1, Func2 f2, Func3 f3, Func4 f4) -> void
        {
            auto e2 = std::move(stack.top());
            stack.pop();
            auto e1 = std::move(stack.top());
            stack.pop();

            if (e1->type == element_type::integer &&
                e2->type == element_type::integer) {

                auto e1_int = std::static_pointer_cast<int_element>(e1);
                auto e2_int = std::static_pointer_cast<int_element>(e2);

                std::shared_ptr<element> e3(
                    new bool_element(f1(e1_int->data, e2_int->data)));
                stack.push(std::move(e3));

            } else if (e1->type == element_type::real &&
                       e2->type == element_type::integer) {

                auto e1_real = std::static_pointer_cast<real_element>(e1);
                auto e2_int = std::static_pointer_cast<int_element>(e2);

                std::shared_ptr<element> e3(
                    new bool_element(f2(e1_real->data, e2_int->data)));
                stack.push(std::move(e3));
            } else if (e1->type == element_type::integer &&
                       e2->type == element_type::real) {

                auto e1_int = std::static_pointer_cast<int_element>(e1);
                auto e2_real = std::static_pointer_cast<real_element>(e2);

                std::shared_ptr<element> e3(
                    new bool_element(f3(e1_int->data, e2_real->data)));
                stack.push(std::move(e3));
            } else {

                auto e1_real = std::static_pointer_cast<real_element>(e1);
                auto e2_real = std::static_pointer_cast<real_element>(e2);

                std::shared_ptr<element> e3(
                    new bool_element(f4(e1_real->data, e2_real->data)));
                stack.push(std::move(e3));
            }
        }

    public:
        auto create_inst(std::vector<std::string> inst_vec)
            -> std::shared_ptr<instruction>
        {
            const std::string op_s = inst_vec[0];
            operation op;

            std::regex clojure("clojure_.:");
            std::regex true_s("true_.:");
            std::regex false_s("false_.:");

            if (op_s == "pop") {
                op = operation::pop;
            } else if (op_s == "apply") {
                op = operation::apply;
            } else if (op_s == "return") {
                op = operation::ret;
            } else if (op_s == "read") {
                op = operation::read;
            } else if (op_s == "write") {
                op = operation::write;
            } else if (op_s == "main:") {
                op = operation::main;
            } else if (std::regex_match(op_s, clojure)) {
                // else if (*(op_s.end() - 1 ) == ':') {
                std::string fn_name = op_s;
                fn_name.erase(fn_name.end() - 1);
                return std::shared_ptr<op_instruction>(new op_instruction(
                    operation::func,
                    std::shared_ptr<element>(new symbol_element(fn_name))));
            } else if (std::regex_match(op_s, true_s) ||
                       std::regex_match(op_s, false_s)) {
                std::string label_name = op_s;
                label_name.erase(label_name.end() - 1);
                return std::shared_ptr<op_instruction>(new op_instruction(
                    operation::func,
                    std::shared_ptr<element>(new symbol_element(label_name))));
            } else if (op_s == "push_int") {
                return std::shared_ptr<op_instruction>(new op_instruction(
                    operation::push_int,
                    std::shared_ptr<element>(
                        new int_element(std::stoi(inst_vec[1])))));
            } else if (op_s == "push_float") {
                return std::shared_ptr<op_instruction>(new op_instruction(
                    operation::push_float,
                    std::shared_ptr<element>(
                        new real_element(std::stof(inst_vec[1])))));
            }

            // else if (op_s == "push_string") {
            //     return instruction(operation::push_string,
            // element(element_type::string, inst_vec[1]));
            // }
            else if (op_s == "push_bool") {
                auto stob = [](std::string s) {
                    return s == "true" ? true : false;
                };

                return std::shared_ptr<op_instruction>(new op_instruction(
                    operation::push_bool,
                    std::shared_ptr<element>(
                        new bool_element(stob(inst_vec[1])))));
            }
            // else if (op_s == "push_list") {
            //     return instruction(operation::push_list,
            // element(element_type::list, inst_vec[1]));
            // }
            else if (op_s == "push_symbol") {
                return std::shared_ptr<op_instruction>(new op_instruction(
                    operation::push_symbol, create_op_element(inst_vec[1])));
            } else if (op_s == "param") {
                return std::shared_ptr<op_instruction>(new op_instruction(
                    operation::param, create_op_element(inst_vec[1])));
            } else if (op_s == "make_list") {
                return std::shared_ptr<op_instruction>(new op_instruction(
                    operation::make_list,
                    std::shared_ptr<element>(
                        new int_element(std::stoi(inst_vec[1])))));
            } else if (op_s == "branch") {
                inst_vec[1].erase(inst_vec[1].end() - 1);
                return std::shared_ptr<op_instruction>(new op_instruction(
                    operation::branch,
                    std::shared_ptr<element>(
                        new branch_element(inst_vec[1], inst_vec[2]))));
            } else {
                op = operation::other;
            }

            return std::shared_ptr<instruction>(new instruction(op));
        }

    public:
        auto create_op_element(const std::string operand)
            -> std::shared_ptr<element>
        {
            element_type type;

            if (operand == "+") {
                type = element_type::add;
            } else if (operand == "-") {
                type = element_type::sub;
            } else if (operand == "*") {
                type = element_type::mul;
            } else if (operand == "/") {
                type = element_type::div;
            } else if (operand == "=") {
                type = element_type::eq;
            } else if (operand == "<") {
                type = element_type::lt;
            } else if (operand == ">") {
                type = element_type::mt;
            } else if (operand == "<=") {
                type = element_type::lte;
            } else if (operand == ">=") {
                type = element_type::mte;
            } else if (operand == "&") {
                type = element_type::land;
            } else if (operand == "|") {
                type = element_type::lor;
            } else if (operand == "define") {
                type = element_type::define;
            } else if (operand == "car") {
                type = element_type::car;
            } else if (operand == "cdr") {
                type = element_type::cdr;
            } else {
                return std::shared_ptr<element>(new symbol_element(operand));
            }

            return std::shared_ptr<element>(new element(type));
        }
    };
    }
}
