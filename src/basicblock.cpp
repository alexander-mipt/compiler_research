#include "basicblock.hpp"
#include "instruction.hpp"
#include <iostream>
#include <set>
namespace IR {

BasicBlock::BasicBlock() {}
BasicBlock::BasicBlock(id_t id) : m_bb_id(id) {}
BasicBlock::BasicBlock(id_t id, InstrInitList instrs, PhyInitList phys) : m_bb_id(id) {
    Instr *prev{nullptr};
    std::set<key_t> instrIds{};
    for (auto *instr : instrs) {
        ASSERT_DEV(instr, "nullptr during bb fill");
        if (!instrIds.insert(instr->get_id()).second) {
            throw std::logic_error("instr id dublicate");
        }
        instr->set_bb(this);
        instr->throwIfNotConsistent_();
        instr->set_prev(prev);
        m_instrs.push_back(instr);
        prev = instr;
    }

    Phy *prevPhy{nullptr};
    for (auto *phy : phys) {
        ASSERT_DEV(phy, "nullptr during bb fill");
        phy->set_bb(this);
        phy->throwIfNotConsistent_();
        phy->set_prev(prevPhy);
        m_phys.push_back(phy);
        prevPhy = phy;
    }
}

BasicBlock::BasicBlock(BasicBlock &&bb) {
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
    Instr *prev = m_instrs.size() ? *instr_clast() : nullptr;
    std::set<key_t> instrIds{};

    for (auto *instr : list) {
        ASSERT_DEV(instr, "nullptr during bb fill");
        if (!instrIds.insert(instr->get_id()).second) {
            throw std::logic_error("instr id dublicate");
        }
        instr->throwIfNotConsistent_();
        instr->set_bb(this);
        instr->set_prev(prev);
        instr->set_next(nullptr);
        if (prev) {
            prev->set_next(instr);
        }
        m_instrs.push_back(instr);
        prev = instr;
    }
}

void BasicBlock::push_phys(BasicBlock::PhyInitList list) {
    Phy *prev = m_phys.size() ? *phy_clast() : nullptr;
    for (auto *phy : list) {
        ASSERT_DEV(phy, "nullptr during bb fill");
        phy->throwIfNotConsistent_();
        m_phys.push_back(phy);
    }
}

void BasicBlock::throwIfNotConsistent_() const {
    const Instr *current_instr = *m_instrs.begin();
    const Instr *prev_instr{nullptr};
    std::set<key_t> instrIds{};
    for (const auto *i : m_instrs) {
        ASSERT_DEV(i, "nullptr instr in bb");
        i->throwIfNotConsistent_();
        i->throwIfWrongInputCount_();
        i->throwIfWrongDeps_();
        if (!i->bb() || i->bb()->get_id() != m_bb_id) {
            throw std::logic_error("wrong bb id");
        }
        if (i->type() == GroupType::PHY) {
            throw std::logic_error("Phy among instr list");
        }
        if (!instrIds.insert(i->get_id()).second) {
            throw std::logic_error("instr id dublicate");
        }
    }
}

std::vector<std::string> BasicBlock::dump_() const {
    std::vector<std::string> strs{};
    auto bb_header = "Basic block; id: " + std::to_string(m_bb_id);
    strs.push_back(bb_header);

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

std::string BasicBlock::dump() const {
    std::stringstream ss{};
    auto output = dump_();
    for (const auto &i : output) {
        ss << i << '\n';
    }
    return ss.str();
}

id_t BasicBlock::set_id(id_t id) {
    m_bb_id = id;
    return m_bb_id;
}

BasicBlock::InstrIt BasicBlock::erase_instr(InstrIt it) {
    if (it == m_instrs.end()) {
        return m_instrs.end();
    }
    auto *instr = *it;
    // clear user list of all inputs
    for (auto inputTt = instr->inputs_begin(); inputTt != instr->inputs_end(); ++inputTt) {
        auto *input = *inputTt;
        input->users().remove(instr);
        // ASSERT_DEV(count == 1, "Dublicates in input users");
    }
    // clear input list of all users
    for (auto userTt = instr->users_begin(); userTt != instr->users_end(); ++userTt) {
        auto *user = *userTt;
        user->inputs().remove(instr);
        // ASSERT_DEV(count == 1, "Dublicates in input users");
    }

    // clear cfg
    auto *next = instr->next();
    auto *prev = instr->prev();
    if (next) {
        next->set_prev(prev);
    }
    if (prev) {
        prev->set_next(next);
    }
    instr->set_next(nullptr);
    instr->set_prev(nullptr);
    instr->set_bb(nullptr);

    InstrIt nextIt = m_instrs.erase(it);
    delete *it;
    return nextIt;
}

std::pair<BasicBlock::InstrIt, Instr *> BasicBlock::cut_instr(key_t key) {
    auto it = m_instrs.begin();
    while (it != m_instrs.end()) {
        auto *instr = *it;
        if (instr->get_id() == key) {
            auto *next = instr->next();
            auto *prev = instr->prev();
            if (next) {
                next->set_prev(prev);
            }
            if (prev) {
                prev->set_next(next);
            }
            instr->set_next(nullptr);
            instr->set_prev(nullptr);
            instr->set_bb(nullptr);
            it = m_instrs.erase(it);
            return {it, instr};
        }
        ++it;
    }
    return {m_instrs.end(), nullptr};
}

BasicBlock::PhyIt BasicBlock::erase_phy(PhyIt it) {
    if (it == m_phys.end()) {
        return m_phys.end();
    }
    PhyIt nextIt = m_phys.erase(it);
    delete *it;
    return nextIt;
}

size_t BasicBlock::removeUnusedInstrs() {
    auto it = m_instrs.begin();
    auto count{0};
    while(it != m_instrs.end()) {
        auto *instr = *it;
        if (instr->type() != IR::GroupType::GENERAL64) {
            ++it;
            continue;
        }
        if (instr->users().size() == 0) {
            it = m_instrs.erase(it);
            ++count;
        } else {
            ++it;
        }
    }
    return count;
}

} // namespace IR