#include "basicblock.hpp"
#include "instruction.hpp"
#include <iostream>
namespace IR {

BasicBlock::~BasicBlock() {
    for (auto ptr : m_instrs) {
        delete ptr;
    }
    for (auto ptr : m_phys) {
        delete ptr;
    }
}



id_t BasicBlock::get_id() const { return m_bb_id; }
BasicBlock::InstrCIt BasicBlock::instr_cbegin() const { return m_instrs.cbegin(); }
BasicBlock::InstrCIt BasicBlock::instr_cend() const { return m_instrs.cend(); }
BasicBlock::PhyCIt BasicBlock::phy_cbegin() const { return m_phys.cbegin(); }
BasicBlock::PhyCIt BasicBlock::phy_cend() const { return m_phys.cend(); }
BasicBlock::InstrIt BasicBlock::instr_begin() { return m_instrs.begin(); }
BasicBlock::InstrIt BasicBlock::instr_end() { return m_instrs.end(); }
BasicBlock::PhyIt BasicBlock::phy_begin() { return m_phys.begin(); }
BasicBlock::PhyIt BasicBlock::phy_end() { return m_phys.end(); }

BasicBlock::BasicBlock(const BasicBlock &&bb) : m_bb_id(bb.m_bb_id) {
    m_instr_ptrs = std::move(bb.m_instr_ptrs);
    m_phi_ptrs = std::move(bb.m_phi_ptrs);
}

BasicBlock::InstrCIt BasicBlock::push_instrs(InstrInitList list) {
        for (auto ptr: list) {
            if (ptr == nullptr) {
                throw std::logic_error("nullptr args");
            }
        }
        Instr* prev{nullptr};
        for (const auto* ptr: list) {
            Instr* instr = new Instr(*ptr);
            instr->set_bb(this);
            instr->set_id(++m_cur_instr_id);
            m_instrs.push_back(instr);
        }
    }

std::vector<std::string> BasicBlock::dump() const {
    std::vector<std::string> strs{};
    strs.push_back("--- Instrs ---");
    for (auto phi : m_phi_ptrs) {
        strs.push_back(phi->dump());
    }
    strs.push_back("--- Phis ---");
    for (auto instr : m_instr_ptrs) {
        strs.push_back(instr->dump());
    }
    return strs;
}

} // namespace IR