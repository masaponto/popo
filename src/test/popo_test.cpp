#include <gtest/gtest.h>
#include <lexical.hpp>
#include <syntax.hpp>
#include <semantic_ir.hpp>
#include <stack_vm.hpp>

#include <list>
#include <stack>
#include <cassert>
#include <memory>

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

TEST(stack_vm, vm_test_0) {
    using namespace popo;
    std::string in_data("\
push_int 10\n\
push_int 5\n\
push_symbol +\n\
apply\n\
");

    stack_vm::vm pvm(in_data);
    pvm.parse();
    EXPECT_EQ("15", pvm.write_element(pvm.stack.top()));
}

TEST(stack_vm, vm_test_1) {
    using namespace popo;
    std::string in_data("\
push_int 10\n\
push_int 5\n\
push_symbol -\n\
apply\n\
");

    stack_vm::vm pvm(in_data);
    pvm.parse();
    EXPECT_EQ("-5", pvm.write_element(pvm.stack.top()));
}

TEST(stack_vm, vm_test_2) {
    using namespace popo;
    std::string in_data("\
push_float 1.400000\n\
push_int 2\n\
push_symbol +\n\
apply\n\
");

    stack_vm::vm pvm(in_data);
    pvm.parse();
    EXPECT_EQ("3.400000", pvm.write_element(pvm.stack.top()));
}

TEST(stack_vm, vm_test_3) {
    using namespace popo;
    std::string in_data("\
push_int 2\n\
push_symbol x\n\
push_symbol define\n\
apply\n\
");

    stack_vm::vm pvm(in_data);
    pvm.parse();
    EXPECT_EQ("2", pvm.write_element(pvm.stack.top()));
}


TEST(stack_vm, vm_test_4) {
    using namespace popo;
    std::string in_data("\
closure_1:\n\
param b\n\
param a\n\
push_symbol b\n\
push_symbol a\n\
push_symbol +\n\
apply\n\
return\n\
push_symbol closure_1\n\
push_symbol f\n\
push_symbol define\n\
apply\n\
push_int 3\n\
push_int 2\n\
push_symbol f\n\
apply\n\
");

    stack_vm::vm pvm(in_data);
    pvm.parse();
    EXPECT_EQ("5", pvm.write_element(pvm.stack.top()));
}


TEST(stack_vm, vm_test_5) {
    using namespace popo;
    std::string in_data("\
push_int 2\n\
push_symbol x\n\
push_symbol define\n\
apply\n\
");

    stack_vm::vm pvm(in_data);
    pvm.parse();
    EXPECT_EQ("2", pvm.write_element(pvm.stack.top()));
}


TEST(stack_vm, vm_test_6) {
    using namespace popo;
    std::string in_data("\
closure_0:\n\
param x\n\
param y\n\
push_symbol y\n\
push_symbol x\n\
push_symbol +\n\
apply\n\
return\n\
closure_1:\n\
param x\n\
param y\n\
push_symbol x\n\
push_symbol y\n\
push_symbol f\n\
apply\n\
return\n\
main:\n\
push_symbol closure_0\n\
push_symbol f\n\
push_symbol define\n\
apply\n\
push_symbol closure_1\n\
push_symbol g\n\
push_symbol define\n\
apply\n\
push_int 3\n\
push_int 2\n\
push_symbol g\n\
apply\n\
");

    stack_vm::vm pvm(in_data);
    pvm.parse();
    EXPECT_EQ("5", pvm.write_element(pvm.stack.top()));
}

TEST(stack_vm, vm_test_7) {
    using namespace popo;
    std::string in_data("\
push_int 2\n\
push_symbol x\n\
push_symbol define\n\
apply\n\
");

    stack_vm::vm pvm(in_data);
    pvm.parse();
    EXPECT_EQ("2", pvm.write_element(pvm.stack.top()));
}


TEST(stack_vm, vm_test_8) {
    using namespace popo;
    std::string in_data("\
push_int 5\n\
push_int 4\n\
push_int 3\n\
push_int 2\n\
make_list 3\n\
push_symbol l\n\
push_symbol define\n\
apply\n\
push_symbol l\n\
push_symbol cdr\n\
apply\n\
push_symbol car\n\
apply\n\
");

    stack_vm::vm pvm(in_data);
    pvm.parse();
    EXPECT_EQ("3", pvm.write_element(pvm.stack.top()));
}


TEST(stack_vm, vm_test_9) {
    using namespace popo;
    std::string in_data("\
false_0:\n\
push_int 2\n\
push_int 6\n\
push_symbol +\n\
apply\n\
return\n\
true_0:\n\
push_int 3\n\
push_int 1\n\
push_symbol +\n\
apply\n\
return\n\
push_int 2\n\
push_int 2\n\
push_symbol =\n\
apply\n\
branch true_0, false_0\n\
");

    stack_vm::vm pvm(in_data);
    pvm.parse();
    EXPECT_EQ("4", pvm.write_element(pvm.stack.top()));
}


TEST(stack_vm, vm_test_10) {
    using namespace popo;
    std::string in_data("\
push_int 5\n\
push_int 3\n\
push_symbol <=\n\
apply\n\
");

    stack_vm::vm pvm(in_data);
    pvm.parse();
    EXPECT_EQ("#f", pvm.write_element(pvm.stack.top()));
}


TEST(stack_vm, vm_test_11) {
    using namespace popo;
    std::string in_data("\
push_int 5\n\
push_symbol x\n\
push_symbol define\n\
apply\n\
push_symbol x\n\
push_symbol x\n\
push_symbol *\n\
apply\n\
push_symbol x\n\
");

    stack_vm::vm pvm(in_data);
    pvm.parse();
    EXPECT_EQ("x", pvm.write_element(pvm.stack.top()));
}


TEST(stack_vm, vm_test_12) {
    using namespace popo;
    std::string in_data("\
push_int 3\n\
push_symbol x\n\
push_symbol define\n\
apply\n\
push_int 5\n\
push_symbol x\n\
push_symbol +\n\
apply\n\
push_int 4\n\
push_symbol x\n\
push_symbol define\n\
apply\n\
push_int 5\n\
push_symbol x\n\
push_symbol +\n\
apply\n\
");

    stack_vm::vm pvm(in_data);
    pvm.parse();
    EXPECT_EQ("9", pvm.write_element(pvm.stack.top()));
}



TEST(stack_vm, vm_test_13) {
    using namespace popo;
    std::string in_data("\
true_0:\n\
push_int 0\n\
return\n\
false_0:\n\
push_int 1\n\
push_symbol a\n\
push_symbol -\n\
apply\n\
write\n\
push_symbol rec\n\
apply\n\
return\n\
closure_0:\n\
param a\n\
push_symbol a\n\
push_int 0\n\
push_symbol <=\n\
apply\n\
branch true_0, false_0\n\
return\n\
push_symbol closure_0\n\
push_symbol rec\n\
push_symbol define\n\
apply\n\
push_int 10\n\
push_symbol rec\n\
apply\n\
");

    stack_vm::vm pvm(in_data);
    pvm.parse();
    EXPECT_EQ("0", pvm.write_element(pvm.stack.top()));
}


TEST(stack_vm, vm_test_14) {
    using namespace popo;
    std::string in_data("\
push_float 3.3\n\
push_string uhihi\n\
push_int 5\n\
push_float 2.22\n\
make_list 4\n\
");

    stack_vm::vm pvm(in_data);
    pvm.parse();
    EXPECT_EQ("2.220000 5 uhihi 3.300000 ", pvm.write_element(pvm.stack.top()));
}



TEST(stack_vm, vm_test_15) {
    using namespace popo;
    std::string in_data("\
push_int 2\n\
push_int 3\n\
push_int 4\n\
push_int 5\n\
make_list 4\n\
push_symbol z\n\
push_symbol define\n\
apply\n\
push_symbol z\n\
push_symbol cdr\n\
apply\n\
write\n\
push_symbol car\n\
apply\n\
write\n\
push_symbol z\n\
write\n\
push_symbol car\n\
apply\n\
write\n\
push_symbol +\n\
apply\n\
write\n\
");

    stack_vm::vm pvm(in_data);
    pvm.parse();
    EXPECT_EQ("9", pvm.write_element(pvm.stack.top()));
}


TEST(stack_vm, vm_test_16) {
    using namespace popo;
    std::string in_data("\
closure_0:\n\
param b\n\
param a\n\
push_symbol b\n\
push_symbol a\n\
push_symbol +\n\
apply\n\
return\n\
push_symbol closure_0\n\
push_symbol f\n\
push_symbol define\n\
apply\n\
write\n\
push_int 3\n\
push_int 2\n\
push_symbol f\n\
apply\n\
write\n\
closure_0:\n\
param b\n\
param a\n\
push_symbol b\n\
push_symbol a\n\
push_symbol *\n\
apply\n\
return\n\
push_symbol closure_0\n\
push_symbol f\n\
push_symbol define\n\
apply\n\
push_int 3\n\
push_int 2\n\
push_symbol f\n\
apply\n\
write\n\
");

    stack_vm::vm pvm(in_data);
    pvm.parse();
    EXPECT_EQ("6", pvm.write_element(pvm.stack.top()));
}



// TEST(ir, ir_test0) {
//     using namespace popo;
//     std::string in_data(
//         "\
//         (+ 1 2)\
//             ");
//     // t0 = 1
//     // t1 = 2
//     // t3 = t0 + t1

//     semantic::semantic_analyzer<std::string> sem(in_data);
//     auto symbol_entry = sem.analyze();
//     while (nullptr != symbol_entry) {
//         symbol_entry = sem.analyze();
//     }


//     auto& ir_men = sem.ir_men;
//     auto instructions = ir_men.get_instructions();
//     auto begin = instructions.begin();

//     EXPECT_EQ(ir::ir_type::assignment, (*begin)->type);
//     auto as = static_cast<ir::assignment*>((*begin++).release());
//     EXPECT_EQ(as->op, ir::assignment::operation::nop);
//     EXPECT_EQ(as->immediate, 1);

//     EXPECT_EQ(ir::ir_type::assignment, (*begin)->type);
//     as = static_cast<ir::assignment*>((*begin++).release());
//     EXPECT_EQ(as->immediate, 2);
//     EXPECT_EQ(as->op, ir::assignment::operation::nop);

//     EXPECT_EQ(ir::ir_type::assignment, (*begin)->type);
//     as = static_cast<ir::assignment*>((*begin++).release());
//     EXPECT_EQ(as->op, ir::assignment::operation::add);

//     EXPECT_EQ(begin, instructions.end());
// }

// TEST(ir, ir_test1) {
//     using namespace popo;
//     std::string in_data(
//         "\
//         (+ 1 (+ 1 2))\
//             ");
//     // t0 = 1
//     // t1 = 1
//     // t2 = 2
//     // t3 = t2 + t1
//     // t4 = t3 + t0

//     semantic::semantic_analyzer<std::string> sem(in_data);
//     auto symbol_entry = sem.analyze();
//     while (nullptr != symbol_entry) {
//         symbol_entry = sem.analyze();
//     }


//     auto& ir_men = sem.ir_men;
//     auto instructions = ir_men.get_instructions();
//     auto begin = instructions.begin();

//     EXPECT_EQ(ir::ir_type::assignment, (*begin)->type);
//     auto as = static_cast<ir::assignment*>((*begin++).release());
//     EXPECT_EQ(as->op, ir::assignment::operation::nop);
//     EXPECT_EQ(as->immediate, 1);

//     EXPECT_EQ(ir::ir_type::assignment, (*begin)->type);
//     as = static_cast<ir::assignment*>((*begin++).release());
//     EXPECT_EQ(as->op, ir::assignment::operation::nop);
//     EXPECT_EQ(as->immediate, 1);

//     EXPECT_EQ(ir::ir_type::assignment, (*begin)->type);
//     as = static_cast<ir::assignment*>((*begin++).release());
//     EXPECT_EQ(as->op, ir::assignment::operation::nop);
//     EXPECT_EQ(as->immediate, 2);

//     EXPECT_EQ(ir::ir_type::assignment, (*begin)->type);
//     as = static_cast<ir::assignment*>((*begin++).release());
//     EXPECT_EQ(as->op, ir::assignment::operation::add);

//     EXPECT_EQ(ir::ir_type::assignment, (*begin)->type);
//     as = static_cast<ir::assignment*>((*begin++).release());
//     EXPECT_EQ(as->op, ir::assignment::operation::add);

//     EXPECT_EQ(begin, instructions.end());

// }

// TEST(ir, ir_test2) {

//     using namespace popo;
//     std::string in_data(
//         "\
//         (define y 10)\
//         (+ 1 y)\
//             ");
// //     t0 = 1
// //     t1 = 10
// //     t2 = t0 + t1

//     semantic::semantic_analyzer<std::string> sem(in_data);
//     auto symbol_entry = sem.analyze();
//     while (nullptr != symbol_entry) {
//         symbol_entry = sem.analyze();
//     }
//     auto& ir_men = sem.ir_men;
//     auto instructions = ir_men.get_instructions();
//     auto begin = instructions.begin();

// //     t0 = 1
//     EXPECT_EQ(ir::ir_type::assignment, (*begin)->type);
//     auto as = static_cast<ir::assignment*>((*begin++).release());
//     EXPECT_EQ(as->op, ir::assignment::operation::nop);
//     EXPECT_EQ(as->immediate, 1);

// //     t1 = 10
//     EXPECT_EQ(ir::ir_type::assignment, (*begin)->type);
//     as = static_cast<ir::assignment*>((*begin++).release());
//     EXPECT_EQ(as->op, ir::assignment::operation::nop);
//     EXPECT_EQ(as->immediate, 10);

// //     t2 = t0 + t1
//     EXPECT_EQ(ir::ir_type::assignment, (*begin)->type);
//     as = static_cast<ir::assignment*>((*begin++).release());
//     EXPECT_EQ(as->op, ir::assignment::operation::add);

//     EXPECT_EQ(begin, instructions.end());
// }

// TEST(ir, ir_test3) {

//     using namespace popo;
//     std::string in_data(
//         "\
//         (if (= 1 2)\
//             (+ 1 2)\
//             (- 2 1))\
//             ");

//     //      t0 = 1
//     //      t1 = 2
//     //      t2 = t0 eq t1
//     //      if t2 goto l0
//     //      t3 = 2
//     //      t4 = 1
//     //      t5 = t3 + t4
//     //      goto l1
//     //  l0:
//     //      t6 = 1
//     //      t7 = 2
//     //      t8 = t7 - t6
//     //  l1:

//     semantic::semantic_analyzer<std::string> sem(in_data);
//     auto symbol_entry = sem.analyze();
//     while (nullptr != symbol_entry) {
//         symbol_entry = sem.analyze();
//     }
//     auto& ir_men = sem.ir_men;
//     auto instructions = ir_men.get_instructions();
//     auto begin = instructions.begin();

//     //      t0 = 1
//     EXPECT_EQ(ir::ir_type::assignment, (*begin)->type);
//     auto as = static_cast<ir::assignment*>((*begin++).release());
//     EXPECT_EQ(as->op, ir::assignment::operation::nop);
//     EXPECT_EQ(as->rop, ir::assignment::relational_op::nop);
//     EXPECT_EQ(as->immediate, 1);
//     //      t1 = 2
//     EXPECT_EQ(ir::ir_type::assignment, (*begin)->type);
//     as = static_cast<ir::assignment*>((*begin++).release());
//     EXPECT_EQ(as->op, ir::assignment::operation::nop);
//     EXPECT_EQ(as->rop, ir::assignment::relational_op::nop);
//     EXPECT_EQ(as->immediate, 2);
//     //      t2 = t0 eq t1
//     EXPECT_EQ(ir::ir_type::assignment, (*begin)->type);
//     as = static_cast<ir::assignment*>((*begin++).release());
//     EXPECT_EQ(as->op, ir::assignment::operation::nop);
//     EXPECT_EQ(as->rop, ir::assignment::relational_op::eq);
//     //      if t2 goto l0
//     EXPECT_EQ(ir::ir_type::branch, (*begin)->type);
//     auto cb = static_cast<ir::condition_branch*>((*begin++).release());
//     EXPECT_EQ(cb->label, "l0");
//     EXPECT_EQ(cb->reg_num, 2);

//     //      t3 = 1
//     EXPECT_EQ(ir::ir_type::assignment, (*begin)->type);
//     as = static_cast<ir::assignment*>((*begin++).release());
//     EXPECT_EQ(as->op, ir::assignment::operation::nop);
//     EXPECT_EQ(as->rop, ir::assignment::relational_op::nop);
//     EXPECT_EQ(as->immediate, 1);
//     //      t4 = 2
//     EXPECT_EQ(ir::ir_type::assignment, (*begin)->type);
//     as = static_cast<ir::assignment*>((*begin++).release());
//     EXPECT_EQ(as->op, ir::assignment::operation::nop);
//     EXPECT_EQ(as->rop, ir::assignment::relational_op::nop);
//     EXPECT_EQ(as->immediate, 2);
//     // t5 = t3 + t4
//     EXPECT_EQ(ir::ir_type::assignment, (*begin)->type);
//     as = static_cast<ir::assignment*>((*begin++).release());
//     EXPECT_EQ(as->op, ir::assignment::operation::add);
//     EXPECT_EQ(as->rop, ir::assignment::relational_op::nop);
//     EXPECT_EQ(as->src_reg0, 3);
//     EXPECT_EQ(as->src_reg1, 4);
//     //      goto l1
//     EXPECT_EQ(ir::ir_type::jmp, (*begin)->type);
//     auto jmp = static_cast<ir::jmp*>((*begin++).release());
//     EXPECT_EQ(jmp->label, "l1");
//     //  l0:
//     EXPECT_EQ(ir::ir_type::label, (*begin)->type);
//     auto lb = static_cast<ir::label*>((*begin++).release());
//     EXPECT_EQ(lb->str, "l0");
//     //      t6 = 1
//     EXPECT_EQ(ir::ir_type::assignment, (*begin)->type);
//     as = static_cast<ir::assignment*>((*begin++).release());
//     EXPECT_EQ(as->op, ir::assignment::operation::nop);
//     EXPECT_EQ(as->rop, ir::assignment::relational_op::nop);
//     EXPECT_EQ(as->immediate, 2);
//     //      t7 = 2
//     EXPECT_EQ(ir::ir_type::assignment, (*begin)->type);
//     as = static_cast<ir::assignment*>((*begin++).release());
//     EXPECT_EQ(as->op, ir::assignment::operation::nop);
//     EXPECT_EQ(as->rop, ir::assignment::relational_op::nop);
//     EXPECT_EQ(as->immediate, 1);
//     //      t8 = t6 + t7
//     EXPECT_EQ(ir::ir_type::assignment, (*begin)->type);
//     as = static_cast<ir::assignment*>((*begin++).release());
//     EXPECT_EQ(as->op, ir::assignment::operation::sub);
//     EXPECT_EQ(as->rop, ir::assignment::relational_op::nop);
//     EXPECT_EQ(as->src_reg0, 6);
//     EXPECT_EQ(as->src_reg1, 7);
//     //  l1:
//     EXPECT_EQ(ir::ir_type::label, (*begin)->type);
//     lb = static_cast<ir::label*>((*begin++).release());
//     EXPECT_EQ(lb->str, "l1");

//     EXPECT_EQ(begin, instructions.end());
//}
