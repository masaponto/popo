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

int main(int argc, char *argv[])
{
    using namespace popo;

    if (argc == 1) {
        popo::repl::repl prepl;
        prepl.run();
    }
    else {
        auto debug = false;
        for (auto i = 1; i < argc; i++) {
            std::string option = argv[i];

            if("-v" == option){
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
//             while (!instruction_list.empty()) {
                pvm.parse(instruction_list);
//                 instruction_list = sa.analyze();
//             }
        }
    }
}
