#pragma once

#include <iostream>
#include <fstream>
#include <iterator>
#include <cassert>
#include <cctype>
#include <string>



namespace popo {

namespace lexical{

enum struct Token { string, symbol, num, left, right, eof, trust, real};

std::string token_text[] = {"string", "symbol", "num",  "left",
                            "right",  "eof", "trust", "real",};

// TODO: when popo read many one line comment, error occured.
template <typename Iteratable>
class lexical_analyzer {
public:
    lexical_analyzer(Iteratable const& ary)
        : begin_(ary.begin()), end_(ary.end()), line_number(1)
    {
    }

private:
    typename Iteratable::const_iterator begin_, end_;

    struct token_value {
        public:
            token_value() : symbol(), str(), num(), trust(), real() {};
            ~token_value() {};

        public:
            std::string symbol;
            std::string str;
            int num;
            bool trust;
            double real;
    } t_val;


public:
    int line_number;

public:

    auto get_next_token(void) -> Token
    {
        // TODO: when code have many one line comment, popo occured error.
        pass_space();
        pass_one_line_comment();

//                             std::cout << *begin_ << std::endl;

        // TODO: add #t and #f procedure
        if (begin_ != end_) {

            // left
            if (*begin_ == '(') {
                begin_++;
                return Token::left;
            }
            // right
            else if (*begin_ == ')') {
//                 std::cout << "line: " << line_number << std::endl;
                begin_++;
                return Token::right;
            }
            // num
            else if ('0' <= *begin_ && *begin_ <= '9') {
                return parse_real_number();
            }
            //  string
            else if ('"' == *begin_) {
                parse_string();
                return Token::string;
            }
            // trust #t, #f
            else if('#' == *begin_){
                parse_trust();
                return Token::trust;
            }
            // symbol
            else if ('!' <= *begin_ && *begin_ <= '~') {
                parse_symbol();
                return Token::symbol;
            }
        }

//         std::cout << "line: " << line_number << std::endl;
        return Token::eof;
    }

    virtual ~lexical_analyzer() = default;

public:
    token_value& get_lex() { return t_val; }

private:
    auto pass_one_line_comment(void) -> void
    {
        while (';' == *begin_) {
            while ('\n' != *++begin_ && begin_ != end_)
                ;
            pass_space();
        }
    }

    auto pass_space(void) -> void
    {
        while ((' ' == *begin_ || '\n' == *begin_) && begin_ != end_) {
            if('\n' == *begin_){
                line_number++;
            }
            begin_++;
        }
    }

    auto parse_real_number() -> Token
    {
        std::string str_num(1, *begin_++);
        while ('0' <= *begin_ && *begin_ <= '9') {
            str_num += *begin_++;
        }

        if ('.' == *begin_++) {
            str_num += ".";
            while ('0' <= *begin_ && *begin_ <= '9') {
                str_num += *begin_++;
            }
            t_val.real = std::stod(str_num);
            return Token::real;
        } else {
            begin_--;
            t_val.num = std::stoi(str_num);
            return Token::num;
        }
    }

    auto parse_symbol() -> void
    {
        std::string str(1, *begin_++);
        while ('!' <= *begin_ && *begin_ <= '~' && '(' != *begin_ &&
               ')' != *begin_) {
            str += *begin_++;
        }
        t_val.symbol = str;
    }

    auto parse_string() -> void
    {
        assert(++begin_ != end_);
        std::string str;
        while ('"' != *begin_) {
            str += *begin_++;
            assert(end_ != begin_);
        }
        t_val.str = str;
        begin_++;
    }

    auto parse_trust() -> void
    {
        if('f' == *(++begin_)){
            t_val.trust = true;
        }
        else if('f' == *begin_){
            t_val.trust = false;
        }
        else {
            assert(false);
        }
        begin_++;
    }
};

}  // namespace lexical
}  // namespace popo
