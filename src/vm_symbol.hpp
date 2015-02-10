#pragma once
#include <string>
#include <list>
#include <memory>

#include "ir_instruction.hpp"

namespace popo {
    namespace stack_vm {

        enum struct sym_class {var, func};

        struct symbol_entry {
        public:
            symbol_entry(sym_class c, std::string n) : sclass(c), name(n) {}
            virtual ~symbol_entry() {}

        public:
            sym_class sclass;
            std::string name;
        };

        struct var_entry : symbol_entry {
        public:
            var_entry(std::string n, std::shared_ptr<element> e)
                : symbol_entry(sym_class::var, n), data(e) {}

        public:
            std::shared_ptr<element> data;
        };

        struct function {
        public :
            //function (std::string n, int num)
            function (std::string n, int num, std::list<std::unique_ptr<instruction>> c)
                : name(n), arg_num(num)
            {
                //code = std::move(c);
            };

        public:
            std::string name;
            int arg_num;
            //std::list<std::unique_ptr<instruction>> code;
        };


        struct func_entry : symbol_entry {
        public:
            func_entry (std::string n, function f)
                : symbol_entry(sym_class::func, n), func(f) {}

        public:
            function func;
        };


    }
}
