#include <gtest/gtest.h>
#include <lexical.hpp>
#include <syntax.hpp>
#include <list>


// lexical_analyser
TEST(lexical_analyser, lex_test_1) {
    using namespace popo::lexical;
    std::string in_data("\
            (+ 1 2)\
            ");

    lexical_analyser<std::string> lex(in_data);

    // (+ 1 2)
    EXPECT_EQ(Token::left, lex.get_next_token());
    EXPECT_EQ(Token::symbol, lex.get_next_token());
    EXPECT_EQ(Token::num, lex.get_next_token());
    EXPECT_EQ(Token::num, lex.get_next_token());
    EXPECT_EQ(Token::right, lex.get_next_token());
    EXPECT_EQ(Token::eof, lex.get_next_token());

}

TEST(lexical_analyser, lex_test_2)
{
    using namespace popo::lexical;
    std::string in_data("\
            (define x 10)\
            ");
    lexical_analyser<std::string> lex(in_data);

    // (define x 10)
    EXPECT_EQ(Token::left, lex.get_next_token());
    EXPECT_EQ(Token::symbol, lex.get_next_token());
    EXPECT_EQ(Token::symbol, lex.get_next_token());
    EXPECT_EQ(Token::num, lex.get_next_token());
    EXPECT_EQ(Token::right, lex.get_next_token());
    EXPECT_EQ(Token::eof, lex.get_next_token());
}

TEST(lexical_analyser, lex_test_3)
{
    using namespace popo::lexical;
    std::string in_data("\
            (define x \"hello\")\
            ");
    lexical_analyser<std::string> lex(in_data);

    // (define y "hello")
    EXPECT_EQ(Token::left, lex.get_next_token());
    EXPECT_EQ(Token::symbol, lex.get_next_token());
    EXPECT_EQ(Token::symbol, lex.get_next_token());
    EXPECT_EQ(Token::string, lex.get_next_token());
    EXPECT_EQ(Token::right, lex.get_next_token());
    EXPECT_EQ(Token::eof, lex.get_next_token());
}

TEST(lexical_analyser, lex_test_4)
{
    using namespace popo::lexical;
    std::string in_data("\
            (define z\
                (lambda (a b)\
                    (+ a b)))\
            ");
    lexical_analyser<std::string> lex(in_data);

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

// s_expression_parser
TEST(syntax_analyzer, syntax_test_1)
{
    using namespace popo::syntax;
    const std::string in_data(
        "\
            (+ 1 2)\
            ");
    s_expression_parser<std::string> parser(in_data);
    auto expr_node = parser.s_exp_parse();
}

// int main(int argc, char **argv) {
//     ::testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }
