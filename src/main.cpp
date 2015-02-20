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

    auto debug = false;
    std::string opt = argv[1];
    if ("-v" == opt) {
        debug = true;
        argc--;
    }

    if (argc == 1) {
        popo::repl::repl prepl;
        prepl.run(debug);
    } else {
        for (auto i = 1; i < argc; i++) {
            std::string option = argv[i];

            if ("-v" == option) {
                debug = true;
                continue;
            }

            std::ifstream fs(option);

            using input_data = std::list<char>;

            const input_data file_data{std::istreambuf_iterator<char>(fs),
                                       std::istreambuf_iterator<char>()};

            semantic::semantic_analyzer<input_data> sa;

            popo::stack_vm::vm pvm;
            auto instruction_list = sa.analyze(file_data, debug);
            pvm.parse(instruction_list);
        }
    }
}
