#include <iostream>
#include <iterator>
#include <fstream>
#include <vector>
#include <list>
#include <boost/range.hpp>

#include "lexer.hpp"


int main()
{
    using namespace popo;

    std::string filename("test.scm");
    std::ifstream fs(filename);

    const std::list<char> file_data{std::istreambuf_iterator<char>(fs),
        std::istreambuf_iterator<char>()};


    lexer::lexical_analyser<std::list<char>::const_iterator>
        lex(file_data.cbegin(), file_data.cend());

}
