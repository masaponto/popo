#include <iostream>
#include <iterator>
#include <fstream>
#include <vector>
#include <list>
#include <cassert>

#include "syntax.hpp"
#include "semantic.hpp"
#include "debug.hpp"


int main()
{
    using namespace popo;

    std::string filename("test.scm");
    std::ifstream fs(filename);

    using input_data = std::list<char>;

    const input_data file_data{std::istreambuf_iterator<char>(fs),
                               std::istreambuf_iterator<char>()};

    semantic::semantic_analyzer<input_data> sa(file_data);

//     while(nullptr != sa.analyze());
    auto symbol_entry = sa.analyze();
    while (nullptr != symbol_entry) {
        symbol_entry = sa.analyze();
    }

//     std::cout << "----- immediate code -----" << std::endl;
//     auto& a = sa.ir_men;
//     for(auto&& in : a.get_instructions()){
//         auto a = in.release();
//         switch (a->type) {
//             case ir::ir_type::assignment: {
//                 auto as = static_cast<ir::assignment*>(a);
//                 std::cout << "  t" << as->dest_reg << " = " << std::flush;
//                 if(as->op == ir::assignment::operation::nop){

//                     if(as->rop == ir::assignment::relational_op::nop){
//                         std::cout << as->immediate << std::endl;
//                     }
//                     else if(as->rop == ir::assignment::relational_op::eq){
//                         std::cout << "t" << as->src_reg0 << " eq t"
//                                   << as->src_reg1 << std::endl;
//                     }

//                 }
//                 else if(as->op == ir::assignment::operation::add){
//                     std::cout << "t" << as->src_reg0 << " + t" << as->src_reg1
//                               << std::endl;
//                 }
//                 else if(as->op == ir::assignment::operation::sub){
//                     std::cout << "t" << as->src_reg0 << " - t" << as->src_reg1
//                               << std::endl;
//                 }

//                 break;
//             }
//             case ir::ir_type::label:
//             {
//                 auto as = static_cast<ir::label*>(a);
//                 std::cout << as->str << ":" << std::endl;
//                 break;
//             }
//             case ir::ir_type::jmp:
//             {
//                 auto as = static_cast<ir::jmp*>(a);
//                 std::cout << "  goto " << as->label << std::endl;
//                 break;
//             }
//             case ir::ir_type::branch:
//             {
//                 auto as = static_cast<ir::condition_branch*>(a);
//                 std::cout << "  if t" << as->reg_num << " goto " << as->label
//                           << std::endl;
//                 break;
//             }
//             case ir::ir_type::param:
//             {
//                 auto pr = static_cast<ir::param*>(a);
//                 std::cout << "  param t" << pr->reg_num << std::endl;
//                 break;
//             }
//             case ir::ir_type::arg:
//             {
//                 break;
//             }
//             case ir::ir_type::call:
//             {
//                 auto cl = static_cast<ir::call*>(a);
//                 std::cout << "  t" << cl->reg_num << " = call " << cl->label
//                           << std::endl;
//                 break;
//             }
//             case ir::ir_type::ret:
//             {
//                 auto re = static_cast<ir::ret*>(a);
//                 std::cout << "  ret t" << re->reg_num << std::endl;
//                 break;
//             }
//         }
//     }
//     std::cout << "----- immediate code end -----" << std::endl;
//     syntax::s_expression_parser<input_data> ep(file_data);

//     auto conscell = ep.s_exp_parse();
//     while (nullptr != conscell) {
//         syntax::print_cons<input_data>(std::move(conscell));
//         std::cout << std::endl;
//         conscell = ep.s_exp_parse();
//     }
}
