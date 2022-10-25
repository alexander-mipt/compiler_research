#include "basicblock.hpp"
#include "instruction.hpp"
#include <iostream>
namespace IR {

BasicBlock::BasicBlock(const BasicBlock &&bb) : m_bb_id(bb.m_bb_id) {
    m_instr_ptrs = std::move(bb.m_instr_ptrs);
    m_phi_ptrs = std::move(bb.m_phi_ptrs);
}

std::vector<std::string> BasicBlock::dump() const {
    std::vector<std::string> strs{};
    strs.push_back("--- Instrs ---");
    for (auto phi: m_phi_ptrs) {
        strs.push_back(phi->dump());
    }
    strs.push_back("--- Phis ---");
    for (auto instr: m_instr_ptrs) {
        strs.push_back(instr->dump());
    }
    return strs;
}

} // namespace IR