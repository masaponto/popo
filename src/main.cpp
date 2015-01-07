#include <iostream>
#include <iterator>
#include <fstream>
#include <vector>
#include <list>
#include <cassert>

#include "syntax.hpp"
#include "semantic.hpp"
#include "debug.hpp"


int main()
{
    using namespace popo;

    std::string filename("test.scm");
    std::ifstream fs(filename);

    using input_data = std::list<char>;

    const input_data file_data{std::istreambuf_iterator<char>(fs),
                               std::istreambuf_iterator<char>()};

    semantic::semantic_analyser<input_data> sa(file_data);

    syntax::s_expression_parser<input_data> ep(file_data);

    auto conscell = ep.s_exp_parse();
    while (nullptr != conscell) {
        syntax::print_cons<input_data>(std::move(conscell));
        std::cout << std::endl;
        conscell = ep.s_exp_parse();
    }
}
