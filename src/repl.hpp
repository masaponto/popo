#include <list>
#include <cassert>
#include <string>

namespace popo {
    namespace repl {

        class repl
        {
        public:
            repl(){};

        public:
            auto run() -> void
            {
                std::cout << "Hello popo REPL" << std::endl;

                std::string in;
                popo::stack_vm::vm pvm;
                //std::regex left("\(.*");
                //std::regex right("\)");
                std::string cons;
                //int count = 0;

                while (true) {
                    std::cout << "popo >> ";
                    int lcount = 0;
                    int rcount = 0;
                    getline(std::cin, in);
                    auto it = in[0];

                    //if (std::regex_match(in,left)) {
                    if (it == '(') {
                        cons += in;
                        lcount+=count_word(in, '(');
                        rcount+=count_word(in, ')');

                        while(lcount != rcount) {
                            getline(std::cin, in);

                            if(*(in.end()) == '\0') {
                                cons += " ";
                            }

                            cons+= in;
                            lcount+=count_word(in, '(');
                            rcount+=count_word(in, ')');
                        }

                        //std::cout << cons << std::endl;
                        semantic::semantic_analyzer<std::string> sa(cons);
                        auto ins_list = sa.analyze();
                        pvm.parse(ins_list);
                        cons = "";

                    }
                    else if(it == '\0') {
                    }
                    else if(in == "q") {
                        break;
                    }
                    else {
                        std::cout << "command not found " << in << " :("<<std::endl;
                    }

                }
            } // run

        private:
            auto count_word(std::string str, char word) -> int
            {
                int count = 0;

                for(auto it = str.begin(); it != str.end(); ++it) {
                    if ((*it) == word) {
                        count++;
                    }
                }
                return count;
            }


        }; // calss repl

    } //namesapace repl
} // namesapace popo
