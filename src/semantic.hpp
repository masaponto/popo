#include <string>
#include <cassert>
#include "syntax.hpp"

namespace popo {
namespace semantic {

// symbol table
enum struct entry_type { value, function, };
class symbol_table_entry {

public:
    symbol_table_entry(entry_type t) : _type(t){};

public:
    entry_type _type;
};

class function_entry : public symbol_table_entry {

public:
    function_entry(std::string n, int arg_num)
        : symbol_table_entry(entry_type::function),
          name(n),
          argument_num(arg_num) {};

private:
    std::string name;
    int argument_num;
};

class value_entry : public symbol_table_entry {

public:
    enum struct value_type { INT, STRING, SYMBOL, };

public:
    value_entry(std::string n, value_type t)
        : symbol_table_entry(entry_type::value), name(n), type(t) {};

private:
    std::string name;
    value_type type;
};

}  // namespace semantic
}  // namespace popo
