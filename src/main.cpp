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
        lex(file_data.begin(), file_data.end());

    while (true) {
        switch (lex.get_next_token()) {
            case lexer::Token::eof:
                std::cout << "eof" << std::endl;
                return 0;

            case lexer::Token::left:
                std::cout << "(";
                break;

            case lexer::Token::right:
                std::cout << ")" << std::endl;
                break;

            case lexer::Token::string:
                std::cout << "string str" <<std::endl;
                break;

            case lexer::Token::num:
                std::cout << "num" << std::endl;
                break;

            default:
                std::cout << "default" << std::endl;
        }
        std::cout << std::flush;
    }
}
