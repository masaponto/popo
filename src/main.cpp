#include <iostream>
#include <iterator>
#include <fstream>
#include <vector>
#include <list>
#include <cassert>
#include <memory>

#include "syntax.hpp"
#include "semantic_ir.hpp"
#include "debug.hpp"
#include "stack_vm.hpp"


auto run_vm(std::string ir_code) -> void
{
    std::cout << std::endl;
    std::cout << "=== src ===" << std::endl;

    //std::cout << ir_code << std::endl;
    popo::stack_vm::vm pvm(ir_code);

    std::cout << "=== result ===" << std::endl;
    pvm.parse();
    std::cout << std::endl;
}


int main()
{
    using namespace popo;

    std::string filename("test.scm");
    std::ifstream fs(filename);

    using input_data = std::list<char>;

    const input_data file_data{std::istreambuf_iterator<char>(fs),
            std::istreambuf_iterator<char>()};

    semantic::semantic_analyzer<input_data> sa(file_data);

    auto instruction_list = sa.analyze();
    while (!instruction_list.empty()) {
        popo::stack_vm::vm pvm;
        pvm.parse(instruction_list);
        instruction_list = sa.analyze();
    }


    std::ifstream ifs("test_ir");
    if (ifs.fail())
    {
        std::cerr << "fail" << std::endl;
        return EXIT_FAILURE;
    }
    std::istreambuf_iterator<char> it(ifs);
    std::istreambuf_iterator<char> last;
    std::string ir_code(it, last);
    //run_vm(ir_code);




//     std::string ir_code3("\
// clojure_0:\n\
// param x\n\
// param y\n\
// push_symbol y\n\
// push_symbol x\n\
// push_symbol +\n\
// apply\n\
// return\n\
// clojure_1:\n\
// param x\n\
// param y\n\
// push_symbol x\n\
// write\n\
// push_symbol y\n\
// write\n\
// push_symbol f\n\
// apply\n\
// return\n\
// main:\n\
// push_symbol clojure_0\n\
// push_symbol f\n\
// push_symbol define\n\
// apply\n\
// push_symbol clojure_1\n\
// push_symbol g\n\
// push_symbol define\n\
// apply\n\
// push_int 3\n\
// push_int 2\n\
// push_symbol a\n\
// push_symbol define\n\
// apply\n\
// write\n\
// push_symbol g\n\
// apply\n\
// write\n\
// ");

//     run_vm(ir_code3);



    //     std::string ir_code4("\
    // main:\n\
    // push_float 3.3\n\
    // push_float 5.22\n\
    // push_float 5.2\n\
    // push_float 2.22\n\
    // make_list 3\n\
    // write\n\
    // ");
    //     run_vm(ir_code4);


    //     std::string ir_code5("\
    // push_float 1.400000\n\
    // push_int 2\n\
    // push_symbol +\n\
    // apply\n\
    // write\n\
    // ");

    //     run_vm(ir_code5);


    //     std::string ir_code6("\
    // main:\n\
    // push_int 3\n\
    // push_int 5\n\
    // push_int 5\n\
    // push_int 2\n\
    // make_list 3\n\
    // write\n\
    // push_symbol l\n\
    // push_symbol define\n\
    // apply\n\
    // push_symbol l\n\
    // push_symbol cdr\n\
    // apply\n\
    // write\n\
    // push_symbol car\n\
    // apply\n\
    // write\n\
    // ");
    //     run_vm(ir_code6);


//     std::string ir_code7("\
// push_int 2\n\
// push_int 3\n\
// push_int 4\n\
// push_int 5\n\
// make_list 4\n\
// push_symbol z\n\
// push_symbol define\n\
// apply\n\
// write\n\
// push_symbol z\n\
// push_symbol cdr\n\
// apply\n\
// write\n\
// push_symbol car\n\
// apply\n\
// write\n\
// push_symbol z\n\
// push_symbol car\n\
// apply\n\
// write\n\
// push_symbol +\n\
// apply\n\
// write\n\
// ");

//run_vm(ir_code7);

//     std::string ir_code8("\
// false_0:\n\
// push_int 2\n\
// push_int 6\n\
// push_symbol +\n\
// apply\n\
// return\n\
// true_0:\n\
// push_int 3\n\
// push_int 1\n\
// push_symbol +\n\
// apply\n\
// return\n\
// push_int 2\n\
// push_int 2\n\
// push_symbol =\n\
// apply\n\
// write\n\
// branch true_0, false_0\n\
// write\n\
// ");

//     run_vm(ir_code8);

    //     std::string ir_code9("\
    // push_int 5\n\
    // push_int 3\n\
    // push_symbol <=\n\
    // apply\n\
    // write\n\
    // ");

    //     run_vm(ir_code9);


    //     run_vm(ir_code8);

//     std::string ir_code10("\
// push_int 5\n\
// push_symbol x\n\
// push_symbol define\n\
// apply\n\
// push_symbol x\n\
// push_int 3\n\
// push_symbol *\n\
// apply\n\
// write\n\
// push_symbol x\n\
// push_symbol x\n\
// push_symbol *\n\
// apply\n\
// write\n\
// push_symbol x\n\
// write\n\
// ");

//         run_vm(ir_code10);

//     std::string ir_code11("\
// true_0:\n\
// push_int 1\n\
// return\n\
// false_0:\n\
// push_symbol x\n\
// push_int 1\n\
// push_symbol -\n\
// apply\n\
// write\n\
// push_symbol f\n\
// apply\n\
// push_symbol x\n\
// push_symbol *\n\
// apply\n\
// return\n\
// clojure_0:\n\
// param x\n\
// push_symbol x\n\
// push_int 0\n\
// push_symbol =\n\
// apply\n\
// branch true_0, false_0\n\
// return\n\
// main:\n\
// push_symbol clojure_0\n\
// push_symbol f\n\
// push_symbol define\n\
// apply\n\
// push_int 10\n\
// push_symbol f\n\
// apply\n\
// write\n\
// return\n\
// ");

//     run_vm(ir_code11);


}
