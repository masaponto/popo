namespace popo {
    namespace stack_vm {

        enum struct element_type {
            integer, real, string, boolean, list,
                add, sub, mul, div, eq,
                lt, mt, lte, mte, land, lor,
                cons, car, cdr,
                func, data, define};
        struct element {
        public :
            element() {}
            element(const element_type t) :type(t) {}
            element(const element_type t, std::string o) :type(t), operand(o) {}

        public:
            element_type type;
            std::string operand;
        };

        struct var {
        public:
            var(std::string l, std::string d, element_type t) : label(l), data(d), type(t) {}
        public:
            std::string label, data;
            element_type type;
        };


        enum struct operate {
            push_int, push_float, push_string, push_symbol, push_bool, push_list,
                pop, main, func, param, apply, ret, make_clojure, branch,
                read, write, make_list
                };
        struct instruction {
        public:
            instruction (operate o) :op(o) {}
            instruction (operate o, element e) :op(o), elm(e) {}
            instruction (operate o, std::string n) :op(o), name(n) {}

        public:
            operate op;
            element elm;
            std::string name;
        };

    }
}
