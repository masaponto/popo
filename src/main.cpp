#include <iostream>
#include <iterator>
#include <fstream>
#include <vector>
#include <list>
#include <cassert>

#include "lexer.hpp"
#include "parser.hpp"

int main()
{
using namespace popo;

    std::string filename("test.scm");
    std::ifstream fs(filename);

    using input_data = std::list<char>;
    const input_data file_data{std::istreambuf_iterator<char>(fs),
                               std::istreambuf_iterator<char>()};

    lexer::lexical_analyser<input_data> lex(file_data);

    parser::s_expression_parser<input_data> ep(lex);
    auto conscell = ep.sexp_parse();

    assert(nullptr != conscell);
    parser::print_cons<input_data>(conscell);

//     while (true) {
//         switch (lex.get_next_token()) {
//             case lexer::Token::eof:
//                 std::cout << "eof" << std::endl;
//                 return 0;

//             case lexer::Token::left:
//                 std::cout << "( " << std::flush;
//                 break;

//             case lexer::Token::right:
//                 std::cout << ")" << std::endl;
//                 break;

//             case lexer::Token::string:
//                 std::cout << lex.get_lex().str << " " << std::flush;
//                 break;

//             case lexer::Token::num:
//                 std::cout << lex.get_lex().num << " " << std::flush;
//                 break;

//             default:
//                 std::cout << "default" << std::endl;
//         }
//     }
}
