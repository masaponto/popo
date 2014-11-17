#include <iostream>
#include <iterator>
#include <fstream>
#include <vector>
#include <list>

#include "lexer.hpp"


int main()
{
    using namespace popo;

    std::string filename("test.scm");
    std::ifstream fs(filename);

    const std::list<char> file_data{std::istreambuf_iterator<char>(fs),
        std::istreambuf_iterator<char>()};

    lexer::lexical_analyser<std::list<char> >
        lex(file_data);

    while (true) {
        switch (lex.get_next_token()) {
            case lexer::Token::eof:
                std::cout << "eof" << std::endl;
                return 0;

            case lexer::Token::left:
                std::cout << "( " << std::flush;
                break;

            case lexer::Token::right:
                std::cout << ")" << std::endl;
                break;

            case lexer::Token::string:
                std::cout << lex.get_lex().str << " " << std::flush;
                break;

            case lexer::Token::num:
                std::cout << lex.get_lex().num << " " << std::flush;
                break;

            default:
                std::cout << "default" << std::endl;
        }
    }
}
