#pragma once

#include <iostream>
#include <fstream>
#include <iterator>
#include <cassert>
#include <cctype>
#include <string>

namespace popo {

    namespace lexer {

        enum struct Token{
            string,
            num,
            left,
            right,
            eof
        };

        struct token_value{
            public:
                token_value(): str(), num(){};
                ~token_value() {};

            public:
                std::string str;
                int num;
        };


        template<typename Iteratable>
        class lexical_analyser
        {
            public:
                lexical_analyser(Iteratable const& ary)
                    :begin_(ary.begin()), end_(ary.end())
                {
                }

                auto get_next_token(void)
                    -> Token
                {
                    pass_space();
                    pass_comment();
//                     std::cout << *begin_ << std::endl;

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
                            parse_digit();
                            return Token::num;
                        }

                        // mark
                        else if('!' <= *begin_ && *begin_ <= '~' ){
                            parse_string();
//                             std::cout << *begin_<< std::endl;
                            return Token::string;
                        }
                    }
                    return Token::eof;
                }

                virtual ~lexical_analyser() = default;

            private:
                typename Iteratable::const_iterator begin_, end_;

            private:
                token_value t_val;

            public:
                token_value& get_lex()
                {
                    return t_val;
                }


            private:
                auto pass_comment(void)
                    -> void
                {
                    if(';' == *begin_){
                        while('\n' != *++begin_ && begin_ != end_)
                            ;
                        pass_space();
                    }
                }

                auto pass_space(void)
                    -> void
                {
                    while ((' ' == *begin_ || '\n' == *begin_) && begin_ != end_) {
                        begin_++;
                    }
                }

                auto parse_digit()
                    -> void
                {
                    std::string str_num(1, *begin_++);
                    while('0' <= *begin_ && *begin_ <= '9'){
                        str_num += *begin_++;
                    }

                    t_val.num = std::stoi(str_num);
                }

                auto parse_string()
                    -> void
                {
                    std::string str(1, *begin_++);
                    while ('!' <= *begin_ && *begin_ <= '~' && '(' != *begin_ &&
                           ')' != *begin_) {
                        str += *begin_++;
                    }
                    t_val.str = str;
                }
        };

    }  // namespace lexer
}  // namespace popo

