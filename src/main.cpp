#include <iostream>
#include "lexer.hpp"


int main()
{
    using namespace popo;

    std::string filename("test.scm");
    lexer::lexical_analyser lex(filename);
}
