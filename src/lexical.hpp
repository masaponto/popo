#pragma once

#include <iostream>
#include <fstream>
#include <iterator>
#include <cassert>
#include <cctype>
#include <string>

namespace popo {

namespace lexical{

enum struct Token { string, symbol, num, left, right, eof, t_true, t_false };

std::string token_text[] = {"string", "symbol", "num",  "left",
                            "right",  "eof",    "true", "false"};

template <typename Iteratable>
class lexical_analyser {
public:
    lexical_analyser(Iteratable const& ary)
        : begin_(ary.begin()), end_(ary.end())
    {
    }

    auto get_next_token(void) -> Token
    {
        // TODO: when code have many one line comment, popo occured error.
        pass_space();
        pass_one_line_comment();
        pass_lines_comment();
        //                     std::cout << *begin_ << std::endl;

        // TODO: add #t and #f procedure
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
                parse_digit();
                return Token::num;
            }
            //  string
            else if ('"' == *begin_) {
                parse_string();
                return Token::string;
            }

            // symbol
            else if ('!' <= *begin_ && *begin_ <= '~') {
                parse_symbol();
                return Token::symbol;
            }
        }
        return Token::eof;
    }

    virtual ~lexical_analyser() = default;

private:
    typename Iteratable::const_iterator begin_, end_;

private:
    struct token_value {
        public:
            token_value() : str(), num() {};
            ~token_value() {};

        public:
            std::string symbol;
            std::string str;
            int num;
    } t_val;

public:
    token_value& get_lex() { return t_val; }

private:
    auto pass_one_line_comment(void) -> void
    {
        if (';' == *begin_) {
            while ('\n' != *++begin_ && begin_ != end_)
                ;
            pass_space();
        }
    }

    auto pass_lines_comment(void) -> void
    {
        if ('#' == *begin_) {
            if ('|' == *(++begin_)) {
                begin_++;
                while (!('|' == *begin_ && '#' == *(++begin_))) {
                    begin_++;
                }
                begin_++;
                pass_space();
                pass_one_line_comment();
            } else {
                begin_--;
            }
        }
    }

    auto pass_space(void) -> void
    {
        while ((' ' == *begin_ || '\n' == *begin_) && begin_ != end_) {
            begin_++;
        }
    }

    auto parse_digit() -> void
    {
        std::string str_num(1, *begin_++);
        while ('0' <= *begin_ && *begin_ <= '9') {
            str_num += *begin_++;
        }

        t_val.num = std::stoi(str_num);
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
};

}  // namespace lexical
}  // namespace popo
