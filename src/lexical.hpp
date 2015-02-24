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
        pass_space();
        pass_one_line_comment();

        if (begin_ != end_) {

            // left
            if (*begin_ == '(') {
                begin_++;
                return Token::left;
            }
            // right
            else if (*begin_ == ')') {
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
            else if('-' == *begin_){
                return parse_minus();
            }
            // symbol
            else if ('!' <= *begin_ && *begin_ <= '~') {
                parse_symbol();
                return Token::symbol;
            }
        }

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

    auto parse_minus() -> Token
    {
        begin_++;
        if('9' >= *begin_ && *begin_ >= '0') {
            auto token = parse_real_number();
            if(Token::num == token){
                t_val.num *= -1;
            }
            else if(Token::real == token){
                t_val.real *= -1;
            }
            else {
                assert(false);
            }
            return token;
        }
        else {
            begin_--;
            return parse_string();
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

    auto parse_string() -> Token
    {
        assert(++begin_ != end_);
        std::string str;
        while ('"' != *begin_) {
            str += *begin_++;
            assert(end_ != begin_);
        }
        t_val.str = str;
        begin_++;
        return Token::string;
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
