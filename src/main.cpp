#include <iostream>
#include <iterator>
#include <fstream>
#include <vector>
#include <list>
#include <cassert>
#include <memory>
#include <string>

#include "syntax.hpp"
#include "semantic_ir.hpp"
#include "debug.hpp"
#include "stack_vm.hpp"
#include "repl.hpp"

auto run_vm(std::string ir_code) -> void
{
    std::cout << "=== src ===" << std::endl;

    std::cout << ir_code << std::endl;

    popo::stack_vm::vm vms(ir_code);

    std::cout << "=== result ===" << std::endl;
    vms.parse();
    std::cout << std::endl;
}

int main(int argc, char *argv[])
{
    using namespace popo;

    // if(argc == 1) {
    //     popo::repl::repl prepl;
    //     prepl.run();
    // }
    // else {
    //     for (auto i = 1; i < argc; i++) {
    //         std::string filename = argv[i];

    //         std::ifstream fs(filename);

    //         using input_data = std::list<char>;

    //         const input_data file_data{std::istreambuf_iterator<char>(fs),
    //                 std::istreambuf_iterator<char>()};

    //         semantic::semantic_analyzer<input_data> sa(file_data);
    //         //sa.analyze();

    //         popo::stack_vm::vm pvm;

    //         auto instruction_list = sa.analyze();
    //         while (!instruction_list.empty()) {
    //             pvm.parse(instruction_list);
    //             instruction_list = sa.analyze();
    //         }
    //     }

    // }


    std::string ir_code3("\
true_0:\n\
push_int 0\n\
return\n\
false_0:\n\
push_int 1\n\
push_symbol a\n\
push_symbol -\n\
apply\n\
write\n\
push_symbol rec\n\
apply\n\
return\n\
closure_0:\n\
param a\n\
push_symbol a\n\
push_int 0\n\
push_symbol <=\n\
apply\n\
branch true_0, false_0\n\
return\n\
push_symbol closure_0\n\
push_symbol rec\n\
push_symbol define\n\
apply\n\
push_int 10\n\
push_symbol rec\n\
apply\n\
");

    run_vm(ir_code3);


    std::string ir_code4("\
push_float 3.3\n\
push_string uhihi\n\
push_int 5\n\
push_float 2.22\n\
make_list 4\n\
write\n\
");
    //run_vm(ir_code4);


    std::string ir_code7("\
push_int 2\n\
push_int 3\n\
push_int 4\n\
push_int 5\n\
make_list 4\n\
push_symbol z\n\
push_symbol define\n\
apply\n\
write\n\
push_symbol z\n\
push_symbol cdr\n\
apply\n\
write\n\
push_symbol car\n\
apply\n\
write\n\
push_symbol z\n\
write\n\
push_symbol car\n\
apply\n\
write\n\
push_symbol +\n\
apply\n\
write\n\
");

    std::string ir_code8("\
closure_0:\n\
param b\n\
param a\n\
push_symbol b\n\
push_symbol a\n\
push_symbol +\n\
apply\n\
return\n\
push_symbol closure_0\n\
push_symbol f\n\
push_symbol define\n\
apply\n\
write\n\
push_int 3\n\
push_int 2\n\
push_symbol f\n\
apply\n\
write\n\
closure_0:\n\
param b\n\
param a\n\
push_symbol b\n\
push_symbol a\n\
push_symbol *\n\
apply\n\
return\n\
push_symbol closure_0\n\
push_symbol f\n\
push_symbol define\n\
apply\n\
push_int 3\n\
push_int 2\n\
push_symbol f\n\
apply\n\
write\n\
");
    //run_vm(ir_code8);

}
