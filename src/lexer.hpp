#pragma once

#include <iostream>
#include <fstream>
#include <cassert>

namespace popo {

    namespace lexer {


//         enum struct Token{
//             variable
//             list
//             list,

//             atom
//             num,
//             string,
//             symbol,
//             nil,

//             left = '(',     // 40
//             right = ')',    // 41

//         }

//         struct Lex{
//             Token token,

//             union Value {
//                 std::string str;
//                 int num;
//                 std::string symbol;

//             }
//         };

        class lexical_analyser
        {
            public:
                lexical_analyser(const std::string &filename) :ifs(filename)
                {
                    assert(ifs);
                }

                ~lexical_analyser()
                {
                    ifs.close();
                }

            private:
                std::ifstream ifs;

        };

    }  // namespace lexer
}  // namespace popo

