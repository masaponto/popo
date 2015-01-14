#pragma once
#include <memory>

namespace popo {
namespace syntax {

    enum struct node_type { num, string, symbol, cons, nil, trust, };

    struct expr_node {
        public:
            expr_node(node_type t) : type(t) {}
//             virtual ~expr_node() {}

        public:
            node_type type;
    };

    struct num_node : public expr_node {
        public:
            num_node(int value) : expr_node(node_type::num), val(value) {}

        public:
            int val;
    };

    struct string_node : public expr_node {
        public:
            string_node(std::string value) : expr_node(node_type::string), val(value) {}

        public:
            std::string val;
    };

    struct symbol_node : public expr_node {
        public:
            symbol_node(std::string value) : expr_node(node_type::symbol), val(value) {}

        public:
            std::string val;
    };

    struct trust_node : public expr_node {
        public:
            trust_node(bool value) : expr_node(node_type::trust), val(value){}

        public:
            bool val;
    };

    struct cons_node : public expr_node {
        public:
            cons_node() : expr_node(node_type::nil)
            {
                car = nullptr;
                cdr = nullptr;
            }

            cons_node(std::unique_ptr<expr_node> ca, std::unique_ptr<expr_node> cd)
                : expr_node(node_type::cons)
            {
                car = std::move(ca);
                cdr = std::move(cd);
            }

        public:
            std::unique_ptr<expr_node> car;
            std::unique_ptr<expr_node> cdr;
    };

}  // namespace syntax
}  // namespace popo
