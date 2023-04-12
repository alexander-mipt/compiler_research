#include "basicblock.hpp"
#include "instruction.hpp"
#include <iostream>
namespace IR {

BasicBlock::BasicBlock() {}
BasicBlock::BasicBlock(id_t id) : m_bb_id(id) {}
BasicBlock::BasicBlock(id_t id, InstrInitList instrs, PhyInitList phys) : m_bb_id(id) {
    Instr *prev{nullptr};
    for (auto *instr : instrs) {
        ASSERT_DEV(instr, "nullptr during bb fill");
        instr->set_bb(this);
        instr->throwIfNotConsistent_();
        instr->set_prev(prev);
        m_instrs.push_back(instr);
        prev = instr;
    }

    Phy *prev{nullptr};
    for (auto *phy : phys) {
        ASSERT_DEV(phy, "nullptr during bb fill");
        phy->set_bb(this);
        phy->throwIfNotConsistent_();
        m_phys.push_back(phy);
    }
}

BasicBlock::BasicBlock(BasicBlock&& bb) {
    for (auto *ptr : bb.m_instrs) {
        m_instrs.push_back(ptr);
    }
    bb.m_instrs.clear();
    for (auto *ptr : bb.m_phys) {
        m_phys.push_back(ptr);
    }
    bb.m_phys.clear();
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
    Instr *prev = m_instrs.size()? *instr_clast() : nullptr;
    for (auto *instr : list) {
        ASSERT_DEV(instr, "nullptr during bb fill");
        instr->throwIfNotConsistent_();
        instr->set_prev(prev);
        m_instrs.push_back(instr);
        prev = instr;
    }
}

void BasicBlock::push_phys(BasicBlock::PhyInitList list) {
    Phy *prev = m_phys.size()? *phy_clast() : nullptr;
    for (auto *phy : list) {
        ASSERT_DEV(phy, "nullptr during bb fill");
        phy->throwIfNotConsistent_();
        m_phys.push_back(phy);
    }
}

void BasicBlock::throwIfNotConsistent_() const {
    const Instr *current_instr = *m_instrs.begin();
    const Instr* prev_instr{nullptr};
    for (const auto *i: m_instrs) {
        ASSERT_DEV(i, "nullptr instr in bb");
        i->throwIfNotConsistent_();
        if (!current_instr || !i || current_instr != i || prev_instr != i->get_prev()) {
            throw std::logic_error("Wrong bb instr sequence");
        }
        prev_instr = i;
        current_instr = static_cast<const Instr*>(i->get_next());
        ASSERT_DEV(current_instr->get_type() != GroupT::PHY, "phy among instr list");
    }
}

std::vector<std::string> BasicBlock::dump() const {
    std::vector<std::string> strs{};

    strs.push_back("--- Phys ---");
    for (const auto *phy : m_phys) {
        strs.push_back(phy->dump());
    }

    strs.push_back("--- Instrs ---");
    for (const auto *instr : m_instrs) {
        strs.push_back(instr->dump());
    }
    return strs;
}

id_t BasicBlock::set_id(id_t id) { m_bb_id = id; }

BasicBlock::InstrIt BasicBlock::erase_instr(InstrIt it) {
    if (it == m_instrs.end()) {
        return m_instrs.end();
    }
    InstrIt nextIt = m_instrs.erase(it);
    delete *it;
    return nextIt;
}

BasicBlock::PhyIt BasicBlock::erase_phy(PhyIt it) {
    if (it == m_phys.end()) {
        return m_phys.end();
    }
    PhyIt nextIt = m_phys.erase(it);
    delete *it;
    return nextIt;
}

} // namespace IR