#include <gtest/gtest.h>
#include <lexical.hpp>
#include <list>

namespace {

class LexicalTest: public ::testing::Test {
    protected:
        LexicalTest() {
        }
};

TEST_F(LexicalTest, l_test_1) {
using namespace popo;

    std::string filename("../../test.scm");
    std::ifstream fs(filename);

    using input_data = std::list<char>;

    const input_data file_data{std::istreambuf_iterator<char>(fs),
                               std::istreambuf_iterator<char>()};


    lexical::lexical_analyser<input_data> lexcal(file_data);
}

} // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
