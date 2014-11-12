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

        template<typename Iterator>
        class lexical_analyser
        {
            public:
                lexical_analyser(Iterator const& b, Iterator const& e)
                    : begin_(b), end_(e)
                {
//                     range_itr = boost::begin(range), end=boost::end(range);
//
//                     assert(is_itr);
                }

                auto get_next_token(void)
                    -> Token
                {
                    std::cout << "end" << *end_ << std::endl;
                    if(begin_ != end_){
                        std::cout << "before:" << *begin_ << std::endl;
                        pass_space();
                        std::cout << "after:" << *begin_ << std::endl;

                        //left
                        if(*begin_ == '('){
                            begin_++;
                            return Token::left;
                        }
                        //right
                        else if(*begin_ == ')'){
                            begin_++;
                            return Token::right;
                        }
                        //num
                        else if('0' <= *begin_ && *begin_ <= '9'){
                            begin_++;
                            parse_digit();
                            return Token::num;
                        }
                        // string
                        else {
                            begin_++;
                            parse_string();
                            return Token::string;
                        }
                    }
                    std::cout << "before" << std::endl;
                    return Token::eof;
                }

                virtual ~lexical_analyser() = default;

            private:
                Iterator begin_, end_;


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
                        if(' ' == *begin_ || '\n' == *begin_){
                            while (' ' == *begin_ || '\n' == *begin_){
                                begin_++;
                            }
                            std::cout << "in pass_space: " << *begin_ << std::endl;
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

