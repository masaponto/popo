#include <gtest/gtest.h>
#include <lexical.hpp>
#include <list>

namespace {

TEST(lexical_analyser, test_1) {
    using namespace popo::lexical;
    std::string in_data(
        "\
            (+ 1 2)\
            (define x 10)\
            (define y \"hello\")\
            (define z \
                (lambda (a b)\
                    (+ a b)))\
            ");

    lexical_analyser<std::string> lex(in_data);

    // (+ 1 2)
    EXPECT_EQ(Token::left, lex.get_next_token());
    EXPECT_EQ(Token::symbol, lex.get_next_token());
    EXPECT_EQ(Token::num, lex.get_next_token());
    EXPECT_EQ(Token::num, lex.get_next_token());
    EXPECT_EQ(Token::right, lex.get_next_token());

    // (define x 10)
    EXPECT_EQ(Token::left, lex.get_next_token());
    EXPECT_EQ(Token::symbol, lex.get_next_token());
    EXPECT_EQ(Token::symbol, lex.get_next_token());
    EXPECT_EQ(Token::num, lex.get_next_token());
    EXPECT_EQ(Token::right, lex.get_next_token());

    // (define y "hello")
    EXPECT_EQ(Token::left, lex.get_next_token());
    EXPECT_EQ(Token::symbol, lex.get_next_token());
    EXPECT_EQ(Token::symbol, lex.get_next_token());
    EXPECT_EQ(Token::string, lex.get_next_token());
    EXPECT_EQ(Token::right, lex.get_next_token());

    // (define z
    //      (lambda (a b)
    //          (+ a b)))
    EXPECT_EQ(Token::left, lex.get_next_token());
    EXPECT_EQ(Token::symbol, lex.get_next_token());
    EXPECT_EQ(Token::symbol, lex.get_next_token());
    EXPECT_EQ(Token::left, lex.get_next_token());
    EXPECT_EQ(Token::symbol, lex.get_next_token());
    EXPECT_EQ(Token::left, lex.get_next_token());
    EXPECT_EQ(Token::symbol, lex.get_next_token());
    EXPECT_EQ(Token::symbol, lex.get_next_token());
    EXPECT_EQ(Token::right, lex.get_next_token());
    EXPECT_EQ(Token::left, lex.get_next_token());
    EXPECT_EQ(Token::symbol, lex.get_next_token());
    EXPECT_EQ(Token::symbol, lex.get_next_token());
    EXPECT_EQ(Token::symbol, lex.get_next_token());
    EXPECT_EQ(Token::right, lex.get_next_token());
    EXPECT_EQ(Token::right, lex.get_next_token());
    EXPECT_EQ(Token::right, lex.get_next_token());

    EXPECT_EQ(Token::eof, lex.get_next_token());
}

} // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
