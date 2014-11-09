#pragma once

#include <iostream>
#include <fstream>
#include <iterator>
#include <cassert>
#include <boost/range.hpp>

namespace popo {

    namespace lexer {


        enum struct Token{
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

        };

//         struct Lex{
//             Token token,

//             union Value {
//                 std::string str;
//                 int num;
//                 std::string symbol;

//             }
//         };
        template<typename Iterator>
        class lexical_analyser
        {
            public:
                lexical_analyser(Iterator const& b, Iterator const& e)
                    : begin_(b), end_(e)
                {
//                     range_itr = boost::begin(range), end=boost::end(range);
//                     assert(is_itr);
                }

                auto get_next_token(void)
                    -> Token
                {
//                     if(begin_ != end_){
//                         return *begin_++;
//                     }
                }

                virtual ~lexical_analyser() = default;

            private:
                Iterator begin_, end_;

        };

    }  // namespace lexer
}  // namespace popo

