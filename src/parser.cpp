#include "parser.hpp"

namespace popo {
    namespace parser {

        auto s_expression_parser::sexp_parse()
            -> expr_node*
        {
            return sexp_car_parse(false);
        }


    } // namespace parser
} // namespace popo
