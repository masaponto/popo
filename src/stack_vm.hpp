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

        class vm
        {
        public:
            vm() {
                symbol_table_list.push_back(global_sym);
            }
            vm (std::string ir_code) : ir_code_ss(ir_code) {
                symbol_table_list.push_back(global_sym);
            }
            ~vm() {}

        private:
            using symbol_table = std::map<std::string, std::shared_ptr<symbol_entry>>;

            std::stringstream ir_code_ss;
            std::list<std::shared_ptr<instruction>> instruction_list;
            std::map<std::string, std::shared_ptr<function>> function_table;

            std::list<symbol_table> symbol_table_list;
            symbol_table global_sym;

        public:
            std::stack<std::shared_ptr<element>> stack;


        public:
            auto parse() -> void
            {
                std::string tmp;
                std::list<std::string> ir_list;
                std::vector<std::string> inst_vec;

                while( std::getline(ir_code_ss, tmp, '\n') ) {
                    ir_list.push_back(tmp);
                }

                for(auto&& ir : ir_list) {
                    std::stringstream inst_ss(ir);

                    while( std::getline(inst_ss, tmp, ' ') ){
                        inst_vec.push_back(tmp);
                    }

                    instruction_list.push_back( create_inst(inst_vec) );
                    inst_vec.clear();
                }

                run();
                instruction_list.clear();
            }


        public:
            auto parse(std::list<std::string> ir_list) -> void
            {
                std::string tmp;
                std::vector<std::string> inst_vec;

                for(auto&& ir : ir_list) {
                    std::stringstream inst_ss(ir);

                    while( std::getline(inst_ss, tmp, ' ') ){
                        inst_vec.push_back(tmp);
                    }

                    instruction_list.push_back( create_inst(inst_vec) );
                    inst_vec.clear();
                }
                run();
                instruction_list.clear();
            }


        public:
            auto run() -> void
            {
                for(auto it = instruction_list.begin(); it != instruction_list.end(); ++it ) {

                    assert(instruction_list.size() != 0);
                    assert(nullptr != (*it));

                    switch((*it)->op) {
                    case operation::main:
                    {
                        break;
                    }
                    case operation::other:
                        {
                            std::cout << "illegal operation" << std::endl;
                            std::exit(1);
                            break;
                        }
                    case operation::func:
                        {
                            auto op_ins = std::static_pointer_cast<op_instruction>(*it);
                            auto func_ele = std::static_pointer_cast<symbol_element>(op_ins->operand);
                            const std::string func_name = func_ele->data;

                            std::list<std::shared_ptr<instruction>> func_code;
                            int arg_num = 0;

                            while( (*it)->op != operation::ret ) {
                                if( (*it)->op == operation::param ) {
                                    arg_num++;
                                }
                                func_code.push_back(std::move(*it));
                                ++it;
                            }

                            function_table.insert( make_pair(func_name, std::shared_ptr<function>
                                                             (new function(func_name, arg_num, std::move(func_code)))) );

                            break;
                        }
                    default:
                        {
                            symbol_table_list = stack_manager(std::move(*it), symbol_table_list);
                        }

                    }
                }
            }


        public:
            auto write_element(std::shared_ptr<element> e) -> std::string
            {
                std::string out;
                assert(nullptr != e);
                switch(e->type) {
                case element_type::integer :
                    {
                        auto e_int = std::static_pointer_cast<int_element>(e);
                        out = std::to_string(e_int->data);
                        break;
                    }
                case element_type::real :
                    {
                        auto e_real = std::static_pointer_cast<real_element>(e);
                        out = std::to_string(e_real->data);
                        break;
                    }
                case element_type::boolean :
                    {
                        auto e_bool = std::static_pointer_cast<bool_element>(e);
                        auto btos = [](bool b){return b ? "#t" : "#f";};
                        out = btos(e_bool->data);
                        break;
                    }
                case element_type::list :
                    {
                        auto e_list = std::static_pointer_cast<list_element>(e);
                        for (auto data : e_list->data) {
                            out +=  write_element(data) + " ";
                        }
                        break;
                    }
                case element_type::symbol :
                    {
                        auto e_symbol = std::static_pointer_cast<symbol_element>(e);
                        out = e_symbol->data;
                        break;
                    }
                case element_type::string :
                    {
                        auto e_string = std::static_pointer_cast<string_element>(e);
                        out = e_string->data;
                        break;
                    }
                default:
                    out = "not implemented write type" ;
                }

                return out;
            }


        public:
            auto stack_manager(std::shared_ptr<instruction> ins,
                               std::list<symbol_table> sym_tables)
                -> std::list<symbol_table>
            {

                switch(ins->op) {
                case operation::write :
                    {
                        if (!stack.empty()) {
                            auto e = stack.top();
                            assert(nullptr != e);
                            std::cout << write_element(std::move(e)) << std::endl;
                        } else {
                            std::cout << "Ooops, stack is empty :(" << std::endl;
                        }

                        break;
                    }
                case operation::push_int :
                case operation::push_float :
                case operation::push_string :
                case operation::push_bool :
                    // case operation::push_list :
                    {
                        auto op_ins = std::static_pointer_cast<op_instruction>(ins);
                        stack.push(op_ins->operand);
                        break;
                    }
                case operation::push_symbol :
                    {
                        auto op_ins = std::static_pointer_cast<op_instruction>(ins);

                        assert(nullptr != op_ins);
                        assert(nullptr != op_ins->operand);

                        if (op_ins->operand->type == element_type::symbol) {
                            auto el = std::static_pointer_cast<symbol_element>(op_ins->operand);
                            bool find_flag = false;

                            for (auto sym_table : sym_tables) {

                                auto sym_it = sym_table.find(el->data);

                                if (sym_it != sym_table.end()) {
                                    find_flag = true;
                                    if (sym_it->second->sclass == sym_class::var) {
                                        auto v = std::static_pointer_cast<var_entry>( sym_it->second );
                                        stack.push( v->data );
                                        //element new_e = *(v->data);
                                        //auto ep = std::make_shared<element>(new_e);
                                        //stack.push(std::move(ep));
                                    } else {
                                        // if function symbol
                                        stack.push(el);
                                    }
                                    break;
                                }
                            }

                            if(!find_flag) {
                                //if new aymbol
                                stack.push(el);
                            }

                        } else {
                            //if other symbol
                            stack.push(op_ins->operand);
                        }

                        break;
                    }
                case operation::pop:
                    {
                        assert(!stack.empty());
                        stack.pop();
                        break;
                    }
                case operation::make_list:
                    {
                        auto op_ins = std::static_pointer_cast<op_instruction>(ins);
                        auto size_e = std::static_pointer_cast<int_element>(op_ins->operand);

                        int size = size_e->data;

                        std::list<std::shared_ptr<element>> e_list;

                        for(int i = 0; i < size; i++) {
                            e_list.push_back( std::move( stack.top() ) );
                            stack.pop();
                        }

                        stack.push(std::shared_ptr<element>( new list_element(e_list)));

                        break;
                    }
                case operation::branch:
                    {
                        auto op_ins = std::static_pointer_cast<op_instruction>(ins);
                        auto branch_e = std::static_pointer_cast<branch_element>(op_ins->operand);
                        auto e = std::move( stack.top() );
                        stack.pop();
                        auto bool_e = std::static_pointer_cast<bool_element>(e);

                        if(bool_e->data)  {
                            auto it = function_table.find(branch_e->t_label);

                            //std::cout << branch_e->t_label << std::endl;
                            //std::cout << function_table.size() << std::endl;
                            assert(it != function_table.end());

                            run_func(it->second->code);
                            //function_table.erase(branch_e->t_label);
                        } else {
                            auto it = function_table.find(branch_e->f_label);
                            run_func(it->second->code);
                            //function_table.erase(branch_e->f_label);
                        }

                        break;
                    }
                case operation::apply :
                    {
                        auto func_e = std::move( stack.top() );
                        stack.pop();

                        switch(func_e->type) {
                        case element_type::add :
                            {
                                auto add_ii = [](int a, int b)->int{ return a + b; };
                                auto add_fi = [](float a, int b)->float{ return a + b; };
                                auto add_if = [](int a, float b)->float{ return a + b; };
                                auto add_ff = [](float a, float b)->float{ return a + b; };
                                calc(add_ii, add_fi, add_if, add_ff);
                                break;
                            }
                        case element_type::sub :
                            {
                                auto sub_ii = [](int a, int b)->int{ return a - b; };
                                auto sub_fi = [](float a, int b)->float{ return a - b; };
                                auto sub_if = [](int a, float b)->float{ return a - b; };
                                auto sub_ff = [](float a, float b)->float{ return a - b; };
                                calc(sub_ii, sub_fi, sub_if, sub_ff);
                                break;
                            }
                        case element_type::mul :
                            {
                                auto mul_ii = [](int a, int b)->int{ return a * b; };
                                auto mul_fi = [](float a, int b)->float{ return a * b; };
                                auto mul_if = [](int a, float b)->float{ return a * b; };
                                auto mul_ff = [](float a, float b)->float{ return a * b; };
                                calc(mul_ii, mul_fi, mul_if, mul_ff);
                                break;
                            }
                        case element_type::div :
                            {
                                auto div_ii = [](int a, int b)->int{ return a / b; };
                                auto div_fi = [](float a, int b)->float{ return a / b; };
                                auto div_if = [](int a, float b)->float{ return a / b; };
                                auto div_ff = [](float a, float b)->float{ return a / b; };
                                calc(div_ii, div_fi, div_if, div_ff);
                                break;
                            }
                        case element_type::eq :
                            {
                                auto eq_ii = [](int a, int b)->int{ return a == b; };
                                auto eq_fi = [](float a, int b)->float{ return a == b; };
                                auto eq_if = [](int a, float b)->float{ return a == b; };
                                auto eq_ff = [](float a, float b)->float{ return a == b; };
                                bool_calc(eq_ii, eq_fi, eq_if, eq_ff);
                                break;
                            }
                        case element_type::lt :
                            {
                                auto lt_ii = [](int a, int b)->int{ return a < b; };
                                auto lt_fi = [](float a, int b)->float{ return a < b; };
                                auto lt_if = [](int a, float b)->float{ return a < b; };
                                auto lt_ff = [](float a, float b)->float{ return a < b; };
                                bool_calc(lt_ii, lt_fi, lt_if, lt_ff);
                                break;
                            }
                        case element_type::mt :
                            {
                                auto mt_ii = [](int a, int b)->int{ return a > b; };
                                auto mt_fi = [](float a, int b)->float{ return a > b; };
                                auto mt_if = [](int a, float b)->float{ return a > b; };
                                auto mt_ff = [](float a, float b)->float{ return a > b; };
                                bool_calc(mt_ii, mt_fi, mt_if, mt_ff);
                                break;
                            }
                        case element_type::lte :
                            {
                                auto lte_ii = [](int a, int b)->int{ return a <= b; };
                                auto lte_fi = [](float a, int b)->float{ return a <= b; };
                                auto lte_if = [](int a, float b)->float{ return a <= b; };
                                auto lte_ff = [](float a, float b)->float{ return a <= b; };
                                bool_calc(lte_ii, lte_fi, lte_if, lte_ff);
                                break;
                            }
                        case element_type::mte :
                            {
                                auto mte_ii = [](int a, int b)->int{ return a >= b; };
                                auto mte_fi = [](float a, int b)->float{ return a >= b; };
                                auto mte_if = [](int a, float b)->float{ return a >= b; };
                                auto mte_ff = [](float a, float b)->float{ return a >= b; };
                                bool_calc(mte_ii, mte_fi, mte_if, mte_ff);
                                break;
                            }
                        case element_type::symbol :
                            {
                                auto symbol_e = std::static_pointer_cast<symbol_element>(func_e);

                                for (auto sym_table : sym_tables) {

                                    auto sym_it = sym_table.find(symbol_e->data);

                                    if (sym_it != sym_table.end()
                                        && sym_it->second->sclass == sym_class::func ) {

                                        auto func_e = std::static_pointer_cast<func_entry>(
                                            sym_it->second );

                                        assert(nullptr != func_e->func);
                                        assert(nullptr != func_e);

                                        run_func(func_e->func->code);
                                        break;
                                    }
                                }

                                break;
                            }
                        case element_type::define :
                            {
                                auto name_e = std::move(stack.top());
                                stack.pop();
                                //std::cout << "abyaaa" << std::endl;
                                assert(!stack.empty());
                                auto data_e = std::move(stack.top());
                                stack.pop();

                                assert(nullptr != name_e);
                                assert(nullptr != data_e);

                                auto sym_name_e = std::static_pointer_cast<symbol_element>(name_e);
                                auto sym = sym_tables.begin();

                                assert(nullptr != sym_name_e);
                                assert(sym != sym_tables.end());
                                //assert(nullptr != sym->second);

                                (*sym) = define(sym_name_e, data_e, (*sym), false);

                                break;
                            }
                        case element_type::cdr:
                            {
                                auto e = std::move(stack.top());
                                stack.pop();

                                auto cons_e = std::static_pointer_cast<list_element>(e);
                                cons_e->data.pop_front();
                                stack.push(std::move(cons_e));

                                break;
                            }
                        case element_type::car:
                            {
                                auto e = std::move(stack.top());
                                stack.pop();

                                auto cons_e = std::static_pointer_cast<list_element>(e);
                                auto i = cons_e->data.front();
                                stack.push(std::move(i));

                                break;
                            }
                        default:
                            {
                                std::cout << "not implemented apply" << std::endl;
                            }

                            break;
                        } // end apply case
                        break;
                    }
                default :
                    {
                        std::cout << "not implemented operation" << std::endl;
                    }
                } // end operation case

                return sym_tables;
            }


            auto define(std::shared_ptr<symbol_element> name_e,
                        std::shared_ptr<element> data_e,
                        symbol_table sym,
                        bool is_param)
                -> symbol_table
            {

                //std::cout << "define called" << std::endl;
                switch(data_e->type) {

                case element_type::integer :
                    {
                        auto e_int = std::static_pointer_cast<int_element>(data_e);
                        std::shared_ptr<symbol_entry> int_var
                            ( new var_entry(name_e->data, std::move(e_int)));
                        sym.insert(make_pair(name_e->data, std::move(int_var)));
                        //std::cout << name_e->data << std::endl;
                        if(!is_param) {
                            stack.push(std::move(data_e));
                        }
                        break;
                    }
                case element_type::real:
                    {
                        auto e_real = std::static_pointer_cast<real_element>(data_e);
                        std::shared_ptr<symbol_entry> real_var
                            ( new var_entry(name_e->data, std::move(e_real)));
                        sym.insert(make_pair(name_e->data, std::move(real_var)));
                        if(!is_param) {
                            stack.push(std::move(data_e));
                        }
                        break;
                    }
                case element_type::symbol:
                    {
                        auto e_func = std::static_pointer_cast<symbol_element>(data_e);
                        auto fn_it = function_table.find(e_func->data);
                        bool find_flag = false;
                        if(fn_it != function_table.end()) {
                            std::shared_ptr<symbol_entry> func
                                ( new func_entry(name_e->data, fn_it->second));
                            sym.insert(make_pair(name_e->data, std::move(func)));
                            find_flag = true;
                            if(!is_param) {
                                stack.push(std::move(name_e));
                            }
                        }
                        else {
                            std::cout << "function " << name_e->data << " is not defined" << std::endl;
                        }

                        break;
                    }
                case element_type::string:
                    {
                        auto e_string = std::static_pointer_cast<string_element>(data_e);
                        //std::cout << e_string->data << std::endl;
                        std::shared_ptr<symbol_entry> string_var
                            ( new var_entry(name_e->data, std::move(e_string)));
                        sym.insert(make_pair(name_e->data, std::move(string_var)));
                        if(!is_param) {
                            stack.push(std::move(data_e));
                        }
                        break;
                    }
                case element_type::list:
                    {
                        auto e_list = std::static_pointer_cast<list_element>(data_e);
                        std::shared_ptr<symbol_entry> list_var
                            ( new var_entry(name_e->data, std::move(e_list)));
                        sym.insert(make_pair(name_e->data, std::move(list_var)));
                        if(!is_param) {
                            stack.push(std::move(data_e));
                        }
                        break;
                    }
                case element_type::boolean:
                    {
                        auto e_bool = std::static_pointer_cast<bool_element>(data_e);
                        std::shared_ptr<symbol_entry> bool_var
                            ( new var_entry(name_e->data, std::move(e_bool)));
                        sym.insert(make_pair(name_e->data, std::move(bool_var)));
                        if(!is_param) {
                            stack.push(std::move(data_e));
                        }
                        break;
                    }
                default:
                    {
                        std::cout << "not implemented define" << std::endl;
                    }

                } //end switch

                return sym;
            }


            auto run_func(std::list<std::shared_ptr<instruction>> code) -> void
            {
                std::map< std::string, std::shared_ptr<symbol_entry>> local_sym;
                auto inst_it = code.begin();
                inst_it++;

                while((*inst_it)->op == operation::param) {
                    auto op_ins = std::static_pointer_cast<op_instruction>(*inst_it);
                    auto symbol_e = std::static_pointer_cast<symbol_element>(op_ins->operand);

                    auto arg_e = std::move(stack.top());
                    stack.pop();
                    local_sym = define(symbol_e, arg_e, local_sym, true);
                    ++inst_it;
                }
                symbol_table_list.push_front(local_sym);

                for (; inst_it != code.end(); ++inst_it) {
                    symbol_table_list = stack_manager(*inst_it, symbol_table_list);
                }

                symbol_table_list.erase(symbol_table_list.begin());
            }


            template<typename Func1, typename Func2, typename Func3, typename Func4>
            auto calc(Func1 f1, Func2 f2, Func3 f3, Func4 f4) -> void
            {
                auto e1 = std::move( stack.top() );
                stack.pop();
                auto e2 = std::move( stack.top() );
                stack.pop();

                if (e1->type == element_type::integer
                    && e2->type == element_type::integer) {

                    auto e1_int = std::static_pointer_cast<int_element>(e1);
                    auto e2_int = std::static_pointer_cast<int_element>(e2);

                    std::shared_ptr<element> e3( new int_element( f1( e1_int->data, e2_int->data ) ) );
                    stack.push(std::move(e3));
                }
                else if (e1->type == element_type::real
                         && e2->type == element_type::integer) {

                    auto e1_real = std::static_pointer_cast<real_element>(e1);
                    auto e2_int = std::static_pointer_cast<int_element>(e2);
                    std::shared_ptr<element> e3( new real_element( f2(e1_real->data, e2_int->data) ) );
                    stack.push(std::move(e3));
                }
                else if (e1->type == element_type::integer
                         && e2->type == element_type::real) {

                    auto e1_int = std::static_pointer_cast<int_element>(e1);
                    auto e2_real = std::static_pointer_cast<real_element>(e2);
                    std::shared_ptr<element> e3( new real_element( f3(e1_int->data, e2_real->data) ) );
                    stack.push(std::move(e3));
                }
                else {

                    auto e1_real = std::static_pointer_cast<real_element>(e1);
                    auto e2_real = std::static_pointer_cast<real_element>(e2);
                    std::shared_ptr<element> e3( new real_element( f4(e1_real->data, e2_real->data) ) );
                    stack.push(std::move(e3));
                }
            }


            template<typename Func1, typename Func2, typename Func3, typename Func4>
            auto bool_calc(Func1 f1, Func2 f2, Func3 f3, Func4 f4) -> void
            {
                auto e2 = std::move( stack.top() );
                stack.pop();
                auto e1 = std::move( stack.top() );
                stack.pop();

                if (e1->type == element_type::integer
                    && e2->type == element_type::integer) {

                    auto e1_int = std::static_pointer_cast<int_element>(e1);
                    auto e2_int = std::static_pointer_cast<int_element>(e2);

                    std::shared_ptr<element> e3( new bool_element( f1( e1_int->data, e2_int->data ) ) );
                    stack.push(std::move(e3));

                }
                else if (e1->type == element_type::real
                         && e2->type == element_type::integer) {

                    auto e1_real = std::static_pointer_cast<real_element>(e1);
                    auto e2_int = std::static_pointer_cast<int_element>(e2);

                    std::shared_ptr<element> e3( new bool_element( f2(e1_real->data, e2_int->data) ) );
                    stack.push(std::move(e3));
                }
                else if (e1->type == element_type::integer
                         && e2->type == element_type::real) {

                    auto e1_int = std::static_pointer_cast<int_element>(e1);
                    auto e2_real = std::static_pointer_cast<real_element>(e2);

                    std::shared_ptr<element> e3( new bool_element( f3(e1_int->data, e2_real->data) ) );
                    stack.push(std::move(e3));
                }
                else {

                    auto e1_real = std::static_pointer_cast<real_element>(e1);
                    auto e2_real = std::static_pointer_cast<real_element>(e2);

                    std::shared_ptr<element> e3( new bool_element( f4(e1_real->data, e2_real->data) ) );
                    stack.push(std::move(e3));
                }
            }


        public:
            auto create_inst(std::vector<std::string> inst_vec) -> std::shared_ptr<instruction>
           {
                const std::string op_s = inst_vec[0];
                operation op;

                std::regex closure("closure_.*:");
                std::regex true_s("true_.*:");
                std::regex false_s("false_.*:");

                if (op_s == "pop") {
                    op = operation::pop;
                }
                else if (op_s == "apply") {
                    op = operation::apply;
                }
                else if (op_s == "return") {
                    op = operation::ret;
                }
                else if (op_s == "read") {
                    op = operation::read;
                }
                else if (op_s == "write") {
                    op = operation::write;
                }
                else if (op_s == "main:") {
                    op = operation::main;
                }
                else if (std::regex_match(op_s, closure)) {
                    std::string fn_name = op_s;
                    fn_name.erase(fn_name.end() - 1);
                    return std::shared_ptr<op_instruction>
                        ( new op_instruction( operation::func, std::shared_ptr<element>
                                              ( new symbol_element( fn_name ) ) ) );
                }
                else if (std::regex_match(op_s, true_s)
                         || std::regex_match(op_s, false_s)) {
                    std::string label_name = op_s;
                    label_name.erase(label_name.end() - 1);
                    return std::shared_ptr<op_instruction>
                        ( new op_instruction( operation::func, std::shared_ptr<element>
                                              ( new symbol_element( label_name ) ) ) );
                }
                else if (op_s == "push_int") {
                    return std::shared_ptr<op_instruction>
                        ( new op_instruction( operation::push_int, std::shared_ptr<element>
                                              ( new int_element( std::stoi( inst_vec[1] ) ) ) ) );
                }
                else if (op_s == "push_float") {
                    return std::shared_ptr<op_instruction>
                        ( new op_instruction( operation::push_float, std::shared_ptr<element>
                                              ( new real_element( std::stof( inst_vec[1] ) ) ) ) );
                }
                else if (op_s == "push_string") {
                    return std::shared_ptr<op_instruction>
                        ( new op_instruction( operation::push_string, std::shared_ptr<element>
                                              ( new string_element( inst_vec[1] ) ) ) );
                }
                else if (op_s == "push_bool") {
                    auto stob = [](std::string s) { return s =="#t" ? true : false; };
                    return std::shared_ptr<op_instruction>
                        ( new op_instruction( operation::push_bool, std::shared_ptr<element>
                                              ( new bool_element( stob( inst_vec[1] ) ) ) ) );
                }
                else if (op_s == "push_symbol") {
                    return std::shared_ptr<op_instruction>
                        ( new op_instruction( operation::push_symbol, create_op_element( inst_vec[1] ) ) );
                }
                else if (op_s == "param") {
                    return std::shared_ptr<op_instruction>
                        ( new op_instruction( operation::param, create_op_element( inst_vec[1] ) ) );
                }
                else if (op_s == "make_list") {
                    return std::shared_ptr<op_instruction>
                        ( new op_instruction( operation::make_list, std::shared_ptr<element>
                                              ( new int_element( std::stoi( inst_vec[1] ) ) ) ) );
                }
                else if (op_s == "branch") {
                    inst_vec[1].erase(inst_vec[1].end() - 1);
                    return std::shared_ptr<op_instruction>
                        ( new op_instruction( operation::branch, std::shared_ptr<element>
                                              ( new branch_element( inst_vec[1], inst_vec[2] ) ) ) );
                }
                else  {
                    op = operation::other;
                }

                return std::shared_ptr<instruction>( new instruction(op) );

            }

        public:
            auto create_op_element(const std::string operand) -> std::shared_ptr<element>
            {
                element_type type;

                if (operand == "+") {
                    type = element_type::add;
                }
                else if (operand == "-") {
                    type = element_type::sub;
                }
                else if (operand == "*") {
                    type = element_type::mul;
                }
                else if (operand == "/") {
                    type = element_type::div;
                }
                else if (operand == "=") {
                    type = element_type::eq;
                }
                else if (operand == "<") {
                    type = element_type::lt;
                }
                else if (operand == ">") {
                    type = element_type::mt;
                }
                else if (operand == "<=") {
                    type = element_type::lte;
                }
                else if (operand == ">=") {
                    type = element_type::mte;
                }
                else if (operand == "&") {
                    type = element_type::land;
                }
                else if (operand == "|") {
                    type = element_type::lor;
                }
                else if (operand == "define") {
                    type = element_type::define;
                }
                else if (operand == "car") {
                    type = element_type::car;
                }
                else if (operand == "cdr") {
                    type = element_type::cdr;
                }
                else {
                    return std::shared_ptr<element>( new symbol_element(operand) );
                    //return std::make_shared<element>(symbol_element(operand));
                }

                return std::shared_ptr<element>( new element(type) );
            }

        };

    }
}
