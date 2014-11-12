#pragma once

#include <iostream>
#include <fstream>
#include <iterator>
#include <cassert>
#include <cctype>

namespace popo {

    namespace lexer {

        enum struct Token{
            string,
            num,
            left,
            right,
            eof
        };

        template<typename Iteratable>
        class lexical_analyser
        {
            public:
                lexical_analyser(Iteratable const& ary)
                    :begin_(ary.begin()), end_(ary.end()), data(ary)
                {
                }

                auto get_next_token(void)
                    -> Token
                {
                    pass_space();
                    if(begin_ != end_){

//                         left
                        if(*begin_ == '('){
                            begin_++;
                            return Token::left;
                        }
//                         right
                        else if(*begin_ == ')'){
                            begin_++;
                            return Token::right;
                        }
//                         num
                        else if('0' <= *begin_ && *begin_ <= '9'){
                            begin_++;
                            parse_digit();
                            return Token::num;
                        }
//                         string
                        else if( ('a' <= *begin_ && *begin_ <= 'z') ||
                                ('A' <= *begin_ && *begin_ <= 'Z')){
                            begin_++;
                            parse_string();
                            return Token::string;
                        }
                        // mark
                        else if('!' <= *begin_ && *begin_ <= '~' ){
                            begin_++;
                            parse_string();
                            return Token::string;
                        }
                    }
                    return Token::eof;
                }

                virtual ~lexical_analyser() = default;

            private:
                typename Iteratable::const_iterator begin_, end_;
                Iteratable data;


            private:
                union token_value{
                    token_value(): str(){};
                    ~token_value(){};
                    std::string str;
                    int num;
                } t_val;


            public:
                auto get_lex()
                    -> token_value
                {
                    return t_val;
                }


            private:
                auto pass_space(void)
                    -> void
                {
                    while (' ' == *begin_ || '\n' == *begin_) {
                        begin_++;
                    }
                }

                auto parse_digit()
                    -> void
                {
                    t_val.num = 0;
                }

                auto parse_string()
                    -> void
                {
                    t_val.str = "a";
                }

        };

    }  // namespace lexer
}  // namespace popo

