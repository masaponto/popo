#include <gtest/gtest.h>
#include <lexical.hpp>
#include <syntax.hpp>
#include <semantic.hpp>
#include <list>

// lexical_analyser
TEST(lexical_analyser, lex_test_1) {
  using namespace popo::lexical;
  std::string in_data("\
            (+ 1 2)\
            ");

  lexical_analyzer<std::string> lex(in_data);

  // (+ 1 2)
  EXPECT_EQ(Token::left, lex.get_next_token());
  EXPECT_EQ(Token::symbol, lex.get_next_token());
  EXPECT_EQ(Token::num, lex.get_next_token());
  EXPECT_EQ(Token::num, lex.get_next_token());
  EXPECT_EQ(Token::right, lex.get_next_token());
  EXPECT_EQ(Token::eof, lex.get_next_token());

}

TEST(lexical_analyser, lex_test_2) {
  using namespace popo::lexical;
  std::string in_data("\
            (define x 10)\
            ");
  lexical_analyzer<std::string> lex(in_data);

  // (define x 10)
  EXPECT_EQ(Token::left, lex.get_next_token());
  EXPECT_EQ(Token::symbol, lex.get_next_token());
  EXPECT_EQ(Token::symbol, lex.get_next_token());
  EXPECT_EQ(Token::num, lex.get_next_token());
  EXPECT_EQ(Token::right, lex.get_next_token());
  EXPECT_EQ(Token::eof, lex.get_next_token());
}

TEST(lexical_analyser, lex_test_3) {
  using namespace popo::lexical;
  std::string in_data("\
            (define x \"hello\")\
            ");
  lexical_analyzer<std::string> lex(in_data);

  // (define y "hello")
  EXPECT_EQ(Token::left, lex.get_next_token());
  EXPECT_EQ(Token::symbol, lex.get_next_token());
  EXPECT_EQ(Token::symbol, lex.get_next_token());
  EXPECT_EQ(Token::string, lex.get_next_token());
  EXPECT_EQ(Token::right, lex.get_next_token());
  EXPECT_EQ(Token::eof, lex.get_next_token());
}

TEST(lexical_analyser, lex_test_4) {
  using namespace popo::lexical;
  std::string in_data("\
            (define z\
                (lambda (a b)\
                    (+ a b)))\
            ");
  lexical_analyzer<std::string> lex(in_data);

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

TEST(lexical_analyser, line_number_test_1)
{
    using namespace popo::lexical;
    std::string in_data(
                "           \
                (define z   \n\
                    (lambda (a b)\n\
                        (+ a b)))\n\
                ; test\
                ");
    lexical_analyzer<std::string> lex(in_data);

    while(Token::eof !=lex.get_next_token());

    EXPECT_EQ(lex.line_number, 4);
}

TEST(lexical_analyser, line_number_test_2){

    using namespace popo::lexical;
  std::string in_data("\
            ; \n \
            ;(define z\n\
            ;    (lambda (a b)\n\
            ;        (+ a b)))\n\
            ; \
            ");
  lexical_analyzer<std::string> lex(in_data);

    while(Token::eof !=lex.get_next_token());

  EXPECT_EQ(lex.line_number, 5);
}


// s_expression_parser
TEST(syntax_analyzer, syntax_test_1) {
  using namespace popo;
  const std::string in_data("\
            (+ 1 2)\
            ");

  // (+ 1 2)
  syntax::s_expression_parser<std::string> parser(in_data);
  auto expr_node = parser.s_exp_parse();

  auto cons_node = std::unique_ptr<syntax::cons_node>(
      static_cast<syntax::cons_node *>(expr_node.release()));

  auto car = std::move(cons_node->car);
  EXPECT_EQ(syntax::node_type::symbol, car->type);

  auto symbol_node = std::unique_ptr<syntax::symbol_node>(
      static_cast<syntax::symbol_node *>(car.release()));
  EXPECT_EQ(symbol_node->val, std::string("+"));

  auto cdr = std::move(cons_node->cdr);
  EXPECT_EQ(syntax::node_type::cons, cdr->type);

  cons_node = std::unique_ptr<syntax::cons_node>(
      static_cast<syntax::cons_node *>(cdr.release()));

  EXPECT_EQ(syntax::node_type::num, cons_node->car->type);

  car = std::move(cons_node->car);

  auto num_node = std::unique_ptr<syntax::num_node>(
      static_cast<syntax::num_node *>(car.release()));

  EXPECT_EQ(num_node->val, 1);

  cdr = std::move(cons_node->cdr);
  EXPECT_EQ(syntax::node_type::cons, cdr->type);

  cons_node = std::unique_ptr<syntax::cons_node>(
      static_cast<syntax::cons_node *>(cdr.release()));

  car = std::move(cons_node->car);
  EXPECT_EQ(syntax::node_type::num, car->type);

  num_node = std::unique_ptr<syntax::num_node>(
      static_cast<syntax::num_node *>(car.release()));

  EXPECT_EQ(num_node->val, 2);

  cdr = std::move(cons_node->cdr);
  EXPECT_EQ(syntax::node_type::nil, cdr->type);
}

// s_expression_parser
TEST(syntax_analyzer, syntax_test_2) {
  using namespace popo;
  std::string in_data("\
            (define x 10)                       \
            ");


  // (define x 10)
  syntax::s_expression_parser<std::string> parser(in_data);
  auto expr_node = parser.s_exp_parse();

  auto cons_node = std::unique_ptr<syntax::cons_node>(
      static_cast<syntax::cons_node *>(expr_node.release()));

  auto car = std::move(cons_node->car);
  EXPECT_EQ(syntax::node_type::symbol, car->type);

  auto symbol_node = std::unique_ptr<syntax::symbol_node>(
      static_cast<syntax::symbol_node *>(car.release()));

  EXPECT_EQ(symbol_node->val, "define");

  auto cdr = std::move(cons_node->cdr);
  EXPECT_EQ(syntax::node_type::cons, cdr->type);

  cons_node = std::unique_ptr<syntax::cons_node>(
      static_cast<syntax::cons_node *>(cdr.release()));

  EXPECT_EQ(syntax::node_type::symbol, cons_node->car->type);

  car = std::move(cons_node->car);

  symbol_node = std::unique_ptr<syntax::symbol_node>(
      static_cast<syntax::symbol_node *>(car.release()));

  EXPECT_EQ(symbol_node->val, "x");

  cdr = std::move(cons_node->cdr);
  EXPECT_EQ(syntax::node_type::cons, cdr->type);

  cons_node = std::unique_ptr<syntax::cons_node>(
      static_cast<syntax::cons_node *>(cdr.release()));

  car = std::move(cons_node->car);
  EXPECT_EQ(syntax::node_type::num, car->type);

  auto num_node = std::unique_ptr<syntax::num_node>(
      static_cast<syntax::num_node *>(car.release()));

  EXPECT_EQ(num_node->val, 10);

  cdr = std::move(cons_node->cdr);
  EXPECT_EQ(syntax::node_type::nil, cdr->type);
}

TEST(syntax_analyzer, syntax_test_3) {
  using namespace popo;

  std::string in_data("\
            (define x \"hello\")\
            ");


  //(define x "hello")
  syntax::s_expression_parser<std::string> parser(in_data);
  auto expr_node = parser.s_exp_parse();

  auto cons_node = std::unique_ptr<syntax::cons_node>(
      static_cast<syntax::cons_node *>(expr_node.release()));

  auto car = std::move(cons_node->car);
  EXPECT_EQ(syntax::node_type::symbol, car->type);

  auto symbol_node = std::unique_ptr<syntax::symbol_node>(
      static_cast<syntax::symbol_node *>(car.release()));

  EXPECT_EQ(symbol_node->val, "define");

  auto cdr = std::move(cons_node->cdr);
  EXPECT_EQ(syntax::node_type::cons, cdr->type);

  cons_node = std::unique_ptr<syntax::cons_node>(
      static_cast<syntax::cons_node *>(cdr.release()));

  EXPECT_EQ(syntax::node_type::symbol, cons_node->car->type);

  car = std::move(cons_node->car);

  symbol_node = std::unique_ptr<syntax::symbol_node>(
      static_cast<syntax::symbol_node *>(car.release()));

  EXPECT_EQ(symbol_node->val, "x");

  cdr = std::move(cons_node->cdr);
  EXPECT_EQ(syntax::node_type::cons, cdr->type);

  cons_node = std::unique_ptr<syntax::cons_node>(
      static_cast<syntax::cons_node *>(cdr.release()));

  car = std::move(cons_node->car);
  EXPECT_EQ(syntax::node_type::string, car->type);

  auto string_node = std::unique_ptr<syntax::string_node>(
      static_cast<syntax::string_node *>(car.release()));

  EXPECT_EQ(string_node->val, "hello");

  cdr = std::move(cons_node->cdr);
  EXPECT_EQ(syntax::node_type::nil, cdr->type);
}

TEST(syntax_analyzer, syntax_test_4) {
  using namespace popo;
  std::string in_data("\
            (define z\
                (lambda (a b)\
                    (+ a b)))\
            ");

  // (define z
  //      (lambda (a b)
  //          (+ a b)))

  syntax::s_expression_parser<std::string> parser(in_data);
  auto expr_node = parser.s_exp_parse();

  auto cons_node = std::unique_ptr<syntax::cons_node>(
      static_cast<syntax::cons_node *>(expr_node.release()));

  auto car = std::move(cons_node->car);
  EXPECT_EQ(syntax::node_type::symbol, car->type);

  auto symbol_node = std::unique_ptr<syntax::symbol_node>(
      static_cast<syntax::symbol_node *>(car.release()));
  EXPECT_EQ(symbol_node->val, "define");

  auto cdr = std::move(cons_node->cdr);
  EXPECT_EQ(syntax::node_type::cons, cdr->type);

  cons_node = std::unique_ptr<syntax::cons_node>(
      static_cast<syntax::cons_node *>(cdr.release()));

  EXPECT_EQ(syntax::node_type::symbol, cons_node->car->type);

  car = std::move(cons_node->car);

  symbol_node = std::unique_ptr<syntax::symbol_node>(
      static_cast<syntax::symbol_node *>(car.release()));

  EXPECT_EQ(symbol_node->val, "z");

  cdr = std::move(cons_node->cdr);
  EXPECT_EQ(syntax::node_type::cons, cdr->type);

  auto cons_node_1 = std::unique_ptr<syntax::cons_node>(
      static_cast<syntax::cons_node *>(cdr.release()));

  car = std::move(cons_node_1->car);
  EXPECT_EQ(syntax::node_type::cons, car->type);

  cons_node = std::unique_ptr<syntax::cons_node>(
      static_cast<syntax::cons_node *>(car.release()));

  car = std::move(cons_node->car);
  EXPECT_EQ(syntax::node_type::symbol, car->type);

  symbol_node = std::unique_ptr<syntax::symbol_node>(
      static_cast<syntax::symbol_node *>(car.release()));

  EXPECT_EQ(symbol_node->val, "lambda");

  cdr = std::move(cons_node->cdr);
  EXPECT_EQ(syntax::node_type::cons, cdr->type);

  auto cons_node_2 = std::unique_ptr<syntax::cons_node>(
      static_cast<syntax::cons_node *>(cdr.release()));

  car = std::move(cons_node_2->car);
  EXPECT_EQ(syntax::node_type::cons, car->type);

  cons_node = std::unique_ptr<syntax::cons_node>(
      static_cast<syntax::cons_node *>(car.release()));

  car = std::move(cons_node->car);
  EXPECT_EQ(syntax::node_type::symbol, car->type);

  symbol_node = std::unique_ptr<syntax::symbol_node>(
      static_cast<syntax::symbol_node *>(car.release()));

  EXPECT_EQ(symbol_node->val, "a");

  cdr = std::move(cons_node->cdr);
  EXPECT_EQ(syntax::node_type::cons, cdr->type);

  cons_node = std::unique_ptr<syntax::cons_node>(
      static_cast<syntax::cons_node *>(cdr.release()));

  car = std::move(cons_node->car);
  EXPECT_EQ(syntax::node_type::symbol, car->type);

  symbol_node = std::unique_ptr<syntax::symbol_node>(
      static_cast<syntax::symbol_node *>(car.release()));

  EXPECT_EQ(symbol_node->val, "b");

  cdr = std::move(cons_node->cdr);
  EXPECT_EQ(syntax::node_type::nil, cdr->type);

  cdr = std::move(cons_node_2->cdr);
  EXPECT_EQ(syntax::node_type::cons, cdr->type);

  cons_node_2 = std::unique_ptr<syntax::cons_node>(
      static_cast<syntax::cons_node *>(cdr.release()));

  car = std::move(cons_node_2->car);
  EXPECT_EQ(syntax::node_type::cons, car->type);

  cons_node = std::unique_ptr<syntax::cons_node>(
      static_cast<syntax::cons_node *>(car.release()));

  car = std::move(cons_node->car);
  EXPECT_EQ(syntax::node_type::symbol, car->type);

  symbol_node = std::unique_ptr<syntax::symbol_node>(
      static_cast<syntax::symbol_node *>(car.release()));

  EXPECT_EQ(symbol_node->val, "+");

  cdr = std::move(cons_node->cdr);
  EXPECT_EQ(syntax::node_type::cons, cdr->type);

  cons_node = std::unique_ptr<syntax::cons_node>(
      static_cast<syntax::cons_node *>(cdr.release()));

  car = std::move(cons_node->car);
  EXPECT_EQ(syntax::node_type::symbol, car->type);

  symbol_node = std::unique_ptr<syntax::symbol_node>(
      static_cast<syntax::symbol_node *>(car.release()));

  EXPECT_EQ(symbol_node->val, "a");

  cdr = std::move(cons_node->cdr);
  EXPECT_EQ(syntax::node_type::cons, cdr->type);

  cons_node = std::unique_ptr<syntax::cons_node>(
      static_cast<syntax::cons_node *>(cdr.release()));

  car = std::move(cons_node->car);
  EXPECT_EQ(syntax::node_type::symbol, car->type);

  symbol_node = std::unique_ptr<syntax::symbol_node>(
      static_cast<syntax::symbol_node *>(car.release()));

  EXPECT_EQ(symbol_node->val, "b");

  cdr = std::move(cons_node->cdr);
  EXPECT_EQ(syntax::node_type::nil, cdr->type);

  cdr = std::move(cons_node_2->cdr);
  EXPECT_EQ(syntax::node_type::nil, cdr->type);

  cdr = std::move(cons_node_1->cdr);
  EXPECT_EQ(syntax::node_type::nil, cdr->type);


}


// s_expression_parser
TEST(syntax_analyzer, syntax_test_5) {
  using namespace popo;
  std::string in_data("\
            ; test \n \
            ; test \n \
            (define x 10)                       \
            ; test \n \
            ; test \n \
            ");


  // (define x 10)
  syntax::s_expression_parser<std::string> parser(in_data);
  auto expr_node = parser.s_exp_parse();

  auto cons_node = std::unique_ptr<syntax::cons_node>(
      static_cast<syntax::cons_node *>(expr_node.release()));

  auto car = std::move(cons_node->car);
  EXPECT_EQ(syntax::node_type::symbol, car->type);

  auto symbol_node = std::unique_ptr<syntax::symbol_node>(
      static_cast<syntax::symbol_node *>(car.release()));

  EXPECT_EQ(symbol_node->val, "define");

  auto cdr = std::move(cons_node->cdr);
  EXPECT_EQ(syntax::node_type::cons, cdr->type);

  cons_node = std::unique_ptr<syntax::cons_node>(
      static_cast<syntax::cons_node *>(cdr.release()));

  EXPECT_EQ(syntax::node_type::symbol, cons_node->car->type);

  car = std::move(cons_node->car);

  symbol_node = std::unique_ptr<syntax::symbol_node>(
      static_cast<syntax::symbol_node *>(car.release()));

  EXPECT_EQ(symbol_node->val, "x");

  cdr = std::move(cons_node->cdr);
  EXPECT_EQ(syntax::node_type::cons, cdr->type);

  cons_node = std::unique_ptr<syntax::cons_node>(
      static_cast<syntax::cons_node *>(cdr.release()));

  car = std::move(cons_node->car);
  EXPECT_EQ(syntax::node_type::num, car->type);

  auto num_node = std::unique_ptr<syntax::num_node>(
      static_cast<syntax::num_node *>(car.release()));

  EXPECT_EQ(num_node->val, 10);

  cdr = std::move(cons_node->cdr);
  EXPECT_EQ(syntax::node_type::nil, cdr->type);
}


TEST(ir, ir_test0) {
    using namespace popo;
    std::string in_data(
        "\
        (+ 1 2)\
            ");
    // t0 = 1
    // t1 = 2
    // t3 = t0 + t1

    semantic::semantic_analyzer<std::string> sem(in_data);
    auto symbol_entry = sem.analyze();
    while (nullptr != symbol_entry) {
        symbol_entry = sem.analyze();
    }


    auto& ir_men = sem.ir_men;
    auto instructions = ir_men.get_instructions();
    auto begin = instructions.begin();

    EXPECT_EQ(ir::ir_type::assignment, (*begin)->type);
    auto as = static_cast<ir::assignment*>((*begin++).release());
    EXPECT_EQ(as->op, ir::assignment::operation::nop);
    EXPECT_EQ(as->immediate, 1);

    EXPECT_EQ(ir::ir_type::assignment, (*begin)->type);
    as = static_cast<ir::assignment*>((*begin++).release());
    EXPECT_EQ(as->immediate, 2);
    EXPECT_EQ(as->op, ir::assignment::operation::nop);

    EXPECT_EQ(ir::ir_type::assignment, (*begin)->type);
    as = static_cast<ir::assignment*>((*begin++).release());
    EXPECT_EQ(as->op, ir::assignment::operation::add);

    EXPECT_EQ(begin, instructions.end());
}

TEST(ir, ir_test1) {
    using namespace popo;
    std::string in_data(
        "\
        (+ 1 (+ 1 2))\
            ");
    // t0 = 1
    // t1 = 1
    // t2 = 2
    // t3 = t2 + t1
    // t4 = t3 + t0

    semantic::semantic_analyzer<std::string> sem(in_data);
    auto symbol_entry = sem.analyze();
    while (nullptr != symbol_entry) {
        symbol_entry = sem.analyze();
    }


    auto& ir_men = sem.ir_men;
    auto instructions = ir_men.get_instructions();
    auto begin = instructions.begin();

    EXPECT_EQ(ir::ir_type::assignment, (*begin)->type);
    auto as = static_cast<ir::assignment*>((*begin++).release());
    EXPECT_EQ(as->op, ir::assignment::operation::nop);
    EXPECT_EQ(as->immediate, 1);

    EXPECT_EQ(ir::ir_type::assignment, (*begin)->type);
    as = static_cast<ir::assignment*>((*begin++).release());
    EXPECT_EQ(as->op, ir::assignment::operation::nop);
    EXPECT_EQ(as->immediate, 1);

    EXPECT_EQ(ir::ir_type::assignment, (*begin)->type);
    as = static_cast<ir::assignment*>((*begin++).release());
    EXPECT_EQ(as->op, ir::assignment::operation::nop);
    EXPECT_EQ(as->immediate, 2);

    EXPECT_EQ(ir::ir_type::assignment, (*begin)->type);
    as = static_cast<ir::assignment*>((*begin++).release());
    EXPECT_EQ(as->op, ir::assignment::operation::add);

    EXPECT_EQ(ir::ir_type::assignment, (*begin)->type);
    as = static_cast<ir::assignment*>((*begin++).release());
    EXPECT_EQ(as->op, ir::assignment::operation::add);

    EXPECT_EQ(begin, instructions.end());

}


// int main(int argc, char **argv) {
//     ::testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }
