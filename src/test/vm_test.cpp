#include "gtest/gtest.h"
#include <stack_vm.hpp>

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
