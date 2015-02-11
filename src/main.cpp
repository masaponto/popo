#include <iostream>
#include <iterator>
#include <fstream>
#include <vector>
#include <list>
#include <cassert>
#include <memory>

#include "syntax.hpp"
#include "semantic.hpp"
#include "debug.hpp"
#include "stack_vm.hpp"


auto run_vm(std::string ir_code) -> void
{
    std::cout << "=== src ===" << std::endl;

    std::cout << ir_code << std::endl;
    popo::stack_vm::vm pvm(ir_code);

    std::cout << "=== result ===" << std::endl;
    pvm.parse();
    std::cout << std::endl;
}



int main()
{
    using namespace popo;

    // std::string filename("test.scm");
    // std::ifstream fs(filename);

    // using input_data = std::list<char>;

    // const input_data file_data{std::istreambuf_iterator<char>(fs),
    //         std::istreambuf_iterator<char>()};

    // semantic::semantic_analyzer<input_data> sa(file_data);


    //     auto symbol_entry = sa.analyze();
    // while (nullptr != symbol_entry) {
    //     symbol_entry = sa.analyze();
    // }


    std::cout << "===== ir ======" << std::endl;
    std::cout << "===vm run ====" << std::endl;

    std::string ir_code1("\
main:\n\
push_int 3\n\
push_int 5\n\
push_symbol +\n\
apply\n\
write\n\
");

    run_vm(ir_code1);

    std::string ir_code1_("\
main:\n\
push_float 3.3\n\
push_float 5.22\n\
push_symbol +\n\
apply\n\
write\n\
");

    run_vm(ir_code1_);

    std::string ir_code2("\
main:\n\
push_float 3.3\n\
push_symbol x\n\
push_symbol define\n\
apply\n\
push_float 2.3\n\
push_symbol y\n\
push_symbol define\n\
apply\n\
push_symbol x\n\
push_symbol y\n\
push_symbol +\n\
apply\n\
write\n\
 ");

    run_vm(ir_code2);


    std::string ir_code3("\
clojure_0:\n\
param x\n\
param y\n\
push_symbol y\n\
push_symbol x\n\
push_symbol +\n\
apply\n\
return\n\
clojure_1:\n\
param x\n\
param y\n\
push_symbol y\n\
push_symbol x\n\
push_symbol f\n\
apply\n\
return\n\
main:\n\
push_symbol clojure_0\n\
push_symbol f\n\
push_symbol define\n\
apply\n\
push_symbol clojure_1\n\
push_symbol g\n\
push_symbol define\n\
apply\n\
push_int 3\n\
push_int 2\n\
push_symbol a\n\
push_symbol define\n\
apply\n\
write\n\
push_symbol a\n\
write\n\
push_symbol g\n\
apply\n\
write\n\
");

    run_vm(ir_code3);



    std::string ir_code4("\
main:\n\
push_float 3.3\n\
push_float 5.22\n\
push_float 5.2\n\
push_float 2.22\n\
make_list 3\n\
write\n\
");
        run_vm(ir_code4);


}
