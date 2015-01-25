#pragma once

#include "node.hpp"
#include "syntax.hpp"

namespace popo {
namespace syntax {

    auto get_space_string(int num) -> std::unique_ptr<std::string>
    {
        std::unique_ptr<std::string> spaces(new std::string('|', 0));
        for (int i = 0; i < num; i++) {
            *spaces += "|";
        }
        return spaces;
    }

    template <typename T>
    auto _print_cons(std::unique_ptr<expr_node> node, int depth, bool is_func)
        -> void
    {
        switch(node->type) {
            case node_type::nil:
                return;

            case node_type::num: {
                std::unique_ptr<num_node> vn(
                    static_cast<num_node*>(node.release()));
                std::cout << *get_space_string(depth) << vn->val << std::endl;
                break;
            }
            case node_type::string: {
                std::unique_ptr<string_node> vn(
                    static_cast<string_node*>(node.release()));

                if (is_func) {
                    std::cout << *get_space_string(depth - 1) << '+'
                              << std::flush;
                } else {
                    std::cout << *get_space_string(depth) << std::flush;
                }
                std::cout << vn->val << std::endl;
                break;
            }

                case node_type::cons: {
                    std::unique_ptr<cons_node> n(
                        static_cast<cons_node*>(node.release()));
                    if (node_type::cons == n->car->type) {
                        _print_cons<T>(std::move(n->car), depth + 1, true);
                    } else {
                        _print_cons<T>(std::move(n->car), depth, is_func);
                    }
                    _print_cons<T>(std::move(n->cdr), depth, false);
                    break;
                }
            default:
                assert(false);
        }

    }

    template <typename T>
    auto print_cons(std::unique_ptr<expr_node> node) -> void
    {
        std::unique_ptr<cons_node> nil(new cons_node());
        std::unique_ptr<cons_node> new_root(
            new cons_node(std::move(node), std::move(nil)));
        _print_cons<T>(std::move(new_root), 0, true);
    }
}  // namespace syntax
}  // namespace popo
