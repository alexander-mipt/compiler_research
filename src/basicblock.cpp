#include "basicblock.hpp"
#include "instruction.hpp"
#include <iostream>
namespace IR {

BasicBlock::BasicBlock() {}
BasicBlock::BasicBlock(id_t id) : m_bb_id(id) {}
BasicBlock::BasicBlock(id_t id, InstrInitList instrs, PhyInitList phys) : m_bb_id(id) {
    for (const auto *ptr : phys) {
        if (ptr == nullptr) {
            throw std::logic_error("nullptr args");
        }
    }
    Instr *prev{nullptr};
    for (const auto *ptr : instrs) {
        Instr *instr = new Instr(*ptr);
        instr->set_bb(this);
        instr->set_id(++m_cur_instr_id);
        m_instrs.push_back(instr);
    }
}

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

BasicBlock::InstrCIt BasicBlock::instr_clast() const {
    if (m_instrs.size() == 0) {
        return m_instrs.cend();
    }
    return --m_instrs.cend();
}
BasicBlock::InstrIt BasicBlock::instr_last() {
    if (m_instrs.size() == 0) {
        return m_instrs.end();
    }
    return --m_instrs.end();
}

BasicBlock::PhyCIt BasicBlock::phy_clast() const {
    if (m_phys.size() == 0) {
        return m_phys.cend();
    }
    return --m_phys.cend();
}
BasicBlock::PhyIt BasicBlock::phy_last() {
    if (m_phys.size() == 0) {
        return m_phys.end();
    }
    return --m_phys.end();
}

void BasicBlock::push_instrs(InstrInitList list) {
    for (auto ptr : list) {
        if (ptr == nullptr) {
            throw std::logic_error("nullptr args");
        }
    }
    Instr *prev = m_instrs.size()? *instr_clast() : nullptr;
    for (const auto *ptr : list) {
        Instr *instr = new Instr(*ptr);
        instr->set_bb(this);
        instr->set_id(++m_cur_instr_id);
        instr->set_prev(prev);
        m_instrs.push_back(instr);
        prev = instr;
    }
}

void BasicBlock::push_phys(BasicBlock::PhyInitList list) {
    for (auto ptr : list) {
        if (ptr == nullptr) {
            throw std::logic_error("nullptr arg");
        }
    }
    Phy *prev = m_phys.size()? *phy_clast() : nullptr;
    for (const auto *ptr : list) {
        Phy *phy = new Phy(*ptr);
        phy->set_bb(this);
        phy->set_id(++m_cur_instr_id);
        m_phys.push_back(phy);
    }
}

void BasicBlock::throwIfNonConsistence_() const {}

std::vector<std::string> BasicBlock::dump() const {
    std::vector<std::string> strs{};

    strs.push_back("--- Instrs ---");
    for (const auto *instr : m_instrs) {
        strs.push_back(instr->dump());
    }

    strs.push_back("--- Phys ---");
    for (const auto *phy : m_phys) {
        strs.push_back(phy->dump());
    }
    return strs;
}

id_t BasicBlock::set_id(id_t id) { m_bb_id = id; }

BasicBlock::InstrCIt BasicBlock::erase_instr(InstrCIt it) {
    if (it == m_instrs.cend()) {
        return m_instrs.cend();
    }
    InstrCIt nextIt = m_instrs.erase(it);
    delete *it;
    return nextIt;
}

BasicBlock::PhyCIt BasicBlock::erase_phy(PhyCIt it) {
    if (it == m_phys.cend()) {
        return m_phys.cend();
    }
    PhyCIt nextIt = m_phys.erase(it);
    delete *it;
    return nextIt;
}

#if 0
BasicBlock::BasicBlock(const BasicBlock &&bb) : m_bb_id(bb.m_bb_id) {
    m_instr_ptrs = std::move(bb.m_instr_ptrs);
    m_phi_ptrs = std::move(bb.m_phi_ptrs);
}

#endif

} // namespace IR