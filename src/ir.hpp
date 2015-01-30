#pragma once
#include <string>
#include <vector>
#include <list>
#include <cassert>
#include <memory>
#include <stack>




namespace popo {
    namespace ir {
    // Three address code

        enum struct ir_type { label, jmp, assignment, operation, branch };
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
        struct assignment : public three_addr_code_base {

            enum struct operation{add, sub, mul, div, nop};
            public:
                assignment(int src0, int src1, int dest, std::string ope)
                    : three_addr_code_base(ir_type::assignment),
                    src_reg0(src0), src_reg1(src1), dest_reg(dest),
                    immediate(0)
                {
                    if(ope == "+"){
                        op = operation::add;
                    }
                    else if(ope == "-"){
                        op = operation::sub;
                    }
                    else if(ope == "*"){
                        op = operation::mul;
                    }
                    else if(ope == "/"){
                        op = operation::div;
                    }
                    else {
                        assert(false);
                    }
                };

                assignment(int imm, int dest)
                    :three_addr_code_base(ir_type::assignment),
                    src_reg0(0), src_reg1(0), dest_reg(dest),
                    op(operation::nop), immediate(imm) {};

            public:
                int src_reg0;
                int src_reg1;
                int dest_reg;
                operation op;
                int immediate;
        };

        // if t0 == t1 goto test
        struct condition_branch : public three_addr_code_base{
            enum struct relational_op{eq, n_eq, gt, lt, ge,le};
            public:
                condition_branch() : three_addr_code_base(ir_type::branch) {};

            public:
                std::string label;
                int reg_num0;
                int reg_num1;
                relational_op rop;
        };

        class ir_manager {
            public:
                ir_manager() : register_size(0), instruction(), symbol()
                               ,instruction_stack(), register_stack() {};

            public:

                auto get_instructions(void)
                    -> std::list<std::unique_ptr<three_addr_code_base>>&&
                {
                    return std::move(instruction);
                }

                auto set_function_symbol(std::string symbol)
                    -> void
                {
                    instruction_stack.push(symbol);
                }

                auto set_immediate(int imm)
                    -> void
                {
                    auto reg_num = assign_imm(imm);
                    register_stack.push(reg_num);
                }

                auto call(int argc = 2)
                    -> int
                {
                    auto function = instruction_stack.top();
                    instruction_stack.pop();

                    auto src1 = register_stack.top();
                    register_stack.pop();
                    auto src0 = register_stack.top();
                    register_stack.pop();
                    return assign_op(src0, src1, function);
                }

            private:

                auto assign_imm(int imm, int dest = -1) -> decltype(dest)
                {
                    if(-1 == dest) {
                        dest = register_size++;
                    }
                    assert(dest >= 0);
                    instruction.push_back(
                            std::unique_ptr<assignment>(
                                new assignment(imm, dest)));
                    return dest;
                }

                auto assign_op(int src0, int src1, std::string op,
                        int dest = -1)
                    -> decltype(dest)
                {
                    if(-1 == dest) {
                        dest = register_size++;
                    }
                    assert(dest >= 0);
                    instruction.push_back(
                            std::unique_ptr<assignment>(
                                new assignment(src0, src1, dest, op)));
                    return dest;
                }



            private:
                int register_size;
                std::list<std::unique_ptr<three_addr_code_base>> instruction;

            private:
                std::string symbol;
                std::stack<std::string> instruction_stack;
                std::stack<int> register_stack;
        };
    }
} // namespace popo
