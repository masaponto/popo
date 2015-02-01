#pragma once
#include <string>
#include <vector>
#include <list>
#include <cassert>
#include <memory>
#include <stack>

#include "semantic.hpp"



namespace popo {
    namespace ir {
    // Three address code

        enum struct ir_type {
            label, jmp, assignment, branch, call, param, arg, ret
        };

        struct three_addr_code_base {
            public:
                three_addr_code_base(ir_type t) : type(t) {};

            public:
                ir_type type;
        };

        // test:
        struct label : public three_addr_code_base {
            public:
                label(std::string s)
                    : three_addr_code_base(ir_type::label), str(s) {};

            public:
                std::string str;
        };

        // goto test
        struct jmp : public three_addr_code_base {
            public:
                jmp(std::string l)
                    : three_addr_code_base(ir_type::jmp), label(l) {};

            public:
                std::string label;
        };

        // t0 = t1 + t2
        // t0 = 2
        // t0 = "hello"
        struct assignment : public three_addr_code_base {

            enum struct operation { add, sub, mul, div, nop };
            enum struct relational_op { eq, n_eq, gt, lt, ge, le, nop };

        public:
            assignment(int src0, int src1, int dest, std::string ope)
                : three_addr_code_base(ir_type::assignment),
                  src_reg0(src0),
                  src_reg1(src1),
                  dest_reg(dest),
                  op(operation::nop),
                  rop(relational_op::nop),
                  immediate(0)
            {

                if (ope == "+") {
                    op = operation::add;
                } else if (ope == "-") {
                    op = operation::sub;
                } else if (ope == "*") {
                    op = operation::mul;
                } else if (ope == "/") {
                    op = operation::div;
                } else if (ope == "=") {
                    rop = relational_op::eq;
                } else {
                    assert(false);
                }
            };

            assignment(int imm, int dest)
                : three_addr_code_base(ir_type::assignment),
                  src_reg0(0),
                  src_reg1(0),
                  dest_reg(dest),
                  op(operation::nop),
                  rop(relational_op::nop),
                  immediate(imm) {};

            // TODO add string constructor
            //                 assignment(std::string s, int dest)
            //                     :three_addr_code_base(ir_type::assignment),
            //                     src_reg0(0), src_reg1(0), dest_reg(dest),
            //                     op(operation::nop), immediate(imm) {};

        public:
            int src_reg0;
            int src_reg1;
            int dest_reg;
            operation op;
            relational_op rop;
            int immediate;
            //                 std::string str;
        };

        // if t0 goto label
        struct condition_branch : public three_addr_code_base{
            public:
                condition_branch(std::string l, int r_num)
                    : three_addr_code_base(ir_type::branch),
                    label(l), reg_num(r_num) {};

            public:
                std::string label;
                int reg_num;
        };


        // t0 = call label
        struct call : public three_addr_code_base {
            public:
                call(std::string l, int reg)
                    : three_addr_code_base(ir_type::call),
                      label(l),
                      reg_num(reg) {};

            public:
                std::string label;
                int reg_num;
        };

        // param t0
        struct param : public three_addr_code_base {
            public:
                param(int reg)
                    : three_addr_code_base(ir_type::param), reg_num(reg){};

            public:
                int reg_num;
        };

        // arg t1
        struct arg : three_addr_code_base{
            public:
                arg(int reg)
                    : three_addr_code_base(ir_type::arg), reg_num(reg) {};

            public:
                int reg_num;
        };

        struct ret : three_addr_code_base{
            public:
                ret(int reg)
                    :three_addr_code_base(ir_type::ret), reg_num(reg) {};

            public:
                int reg_num;
        };


        class ir_manager {
            public:
                ir_manager()
                    : register_size(0),
                      instruction(),
                      label_count(0),
                      symbol(),
                      instruction_stack(),
                      register_stack(),
                      is_define(false) {};

            public:
                auto get_instructions(void)
                    -> std::list<std::unique_ptr<three_addr_code_base>> &&
                {
                    return std::move(instruction);
                }

                auto set_function_symbol(std::string symbol) -> void
                {
                    if(is_define){
                        return;
                    }
                    instruction_stack.push(symbol);
                }

                auto set_immediate(int imm) -> void
                {
                    if(is_define){
                        return;
                    }
                    auto reg_num = assign_imm(imm);
                    register_stack.push(reg_num);
                }

                auto set_label(std::string l) -> void
                {
                    instruction.push_back(std::unique_ptr<label>(
                        new label(std::to_string(register_size) + l)));
                }

                auto set_return() -> void
                {
                    instruction.push_back(
                            std::unique_ptr<ret>(new ret(register_size-1)));
                }

                auto call_function(int argc = 2) -> int
                {
                    if (is_define) {
                        return -1;
                    }
                    auto function = instruction_stack.top();
                    instruction_stack.pop();

                    for(int i=0; i<argc; ++i){
                        auto p = register_stack.top();
                        register_stack.pop();
                        instruction.push_back(
                            std::unique_ptr<param>(new param(p)));
                    }

//                     auto src1 = register_stack.top();
//                     register_stack.pop();
//                     auto src0 = register_stack.top();
//                     register_stack.pop();

//                     instruction.push_back(
//                         std::unique_ptr<param>(new param(src1)));

//                     instruction.push_back(
//                         std::unique_ptr<param>(new param(src0)));

                    auto dest = register_size++;
                    instruction.push_back(std::unique_ptr<call>(
                        new call(function, dest)));

//                     auto dest = assign_op(src0, src1, function);
                    register_stack.push(dest);
                    return dest;
                }

                auto disable_instruction() -> void
                {
                    assert(is_define == false);
                    is_define = true;
                }

                auto enable_instruction() -> void
                {
                    assert(is_define == true);
                    is_define = false;
                }

                auto if_call()
                    -> int
                {
                    std::string label0(1, 'l');
                    label0 += std::to_string(label_count++);

                    instruction.push_back(
                        std::unique_ptr<condition_branch>(
                            new condition_branch(label0, (register_size - 1))));

                    return label_count++;
                }

                auto if_second(int l_count)
                    -> void
                {
                    std::string label0(1, 'l');
                    label0 += std::to_string(l_count-1);

                    std::string label1(1, 'l');
                    label1 += std::to_string(l_count);

                    instruction.push_back(
                        std::unique_ptr<jmp>(new jmp(label1)));

                    instruction.push_back(
                        std::unique_ptr<label>(new label(label0)));
                }

                auto if_last(int l_count)
                    -> void
                {
                    std::string label0(1, 'l');
                    label0 += std::to_string(l_count);

                    instruction.push_back(
                        std::unique_ptr<label>(new label(label0)));
                }

            private:
                auto assign_imm(int imm, int dest = -1) -> decltype(dest)
                {
                    if (-1 == dest) {
                        dest = register_size++;
                    }
                    assert(dest >= 0);
                    instruction.push_back(
                        std::unique_ptr<assignment>(new assignment(imm, dest)));
                    return dest;
                }

                auto assign_op(int src0, int src1, std::string op,
                        int dest = -1)
                    -> decltype(dest)
                {
                    if (-1 == dest) {
                        dest = register_size++;
                    }
                    assert(dest >= 0);
                    instruction.push_back(std::unique_ptr<assignment>(
                        new assignment(src0, src1, dest, op)));
                    return dest;
                }

            private:
                int register_size;
                std::list<std::unique_ptr<three_addr_code_base>> instruction;
                int label_count;

            private:
                std::string symbol;
                std::stack<std::string> instruction_stack;
                std::stack<int> register_stack;

                bool is_define;
        };
    }
} // namespace popo
