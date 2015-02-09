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

    std::string filename("test.scm");
    std::ifstream fs(filename);

    using input_data = std::list<char>;

    const input_data file_data{std::istreambuf_iterator<char>(fs),
            std::istreambuf_iterator<char>()};

    semantic::semantic_analyzer<input_data> sa(file_data);


        auto symbol_entry = sa.analyze();
    while (nullptr != symbol_entry) {
        symbol_entry = sa.analyze();
    }


    std::cout << "===== ir ======" << std::endl;
    std::cout << "===vm run ====" << std::endl;

    std::string ir_code1("\
main:\n\
\tpush_int 3\n\
\tpush_int 5\n\
\tpush_symbol +\n\
\tapply\n\
\twrite\n\
");

    run_vm(ir_code1);

    std::string ir_code1_("\
main:\n\
\tpush_int 3\n\
\tpush_float 5.22\n\
\tpush_symbol /\n\
\tapply\n\
\twrite\n\
");

    //run_vm(ir_code1_);


    std::string ir_code2("\
main:\n\
\tpush_int 3\n\
\tpush_symbol x\n\
\tpush_symbol define\n\
\tapply\n\
\tpush_symbol x\n\
\twrite\n\
 ");

    //std::cout << ir_code2 << std::endl;

    run_vm(ir_code2);


    std::string ir_code3("\
clojure_0:\n\
\tparam x\n\
\tparam y\n\
\tpush_symbol y\n\
\tpush_symbol x\n\
\tpush_symbol +\n\
\tapply\n\
\treturn\n\
main:\n\
\tpush_symbol clojure_0\n\
\tpush_symbol f\n\
\tpush_symbol define\n\
\tapply\n\
\tpush_int 3\n\
\tpush_int 4\n\
\tpush_symbol f\n\
\tapply\n\
\twrite\n\
");

    //std::cout << ir_code3 << std::endl;

    run_vm(ir_code3);

}
