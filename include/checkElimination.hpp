#pragma once
#include "bbGraph.hpp"
#include "instruction.hpp"
#include "memManager.hpp"

bool dominativeCheckAnalysis(IR::Instr *prev, IR::Instr* cur, IR::BbGraph &g) {
    ASSERT_DEV(prev->m_type == IR::GroupType::CHECK && cur->m_type == IR::GroupType::CHECK, "Not check instr");
    if (prev->m_opcd != cur->m_opcd) {
        return false;
    }
    auto it0 = prev->inputs_begin();
    ASSERT_DEV(cur->inputs().size() == prev->inputs().size(), "Wrong number of inputs");
    for (auto it = cur->inputs_begin(); it != cur->inputs_end(); ++it) {
        if (*it != *it0) {
            return false;
        }
        ++it0;
    }

    auto prev_bb_key = prev->bb()->get_id();
    auto cur_bb_key = cur->bb()->get_id();
    if (prev_bb_key == cur_bb_key) {
        return true;
    }

    return g.is_a_dominates_b(g.accessHeader()->get_key(), prev_bb_key, cur_bb_key);
}


/*
Check elimination algo:
    1) get RPO sequence
    2) collect CHECK instrs in RPO order
    3) for each CHECK make a look in previous checks and:
    4) if current & prev CHECK have same inputs and instr_ids then check its domination
    5) if its bbs are the same - exclude the youngest check; else:
    6) if one bb dominates the other - exclude the youngest check. 
*/
void doCheckElimination(IR::BbGraph &g) {
    std::list<IR::Instr *> checks{};
    auto sequence = g.RPO(g.accessHeader()->get_key());
    for (auto key : sequence) {
        auto &bb = g.at(key)->data();
        for (auto it = bb.instr_begin(); it != bb.instr_end(); ++it) {
            auto *instr = *it;
            ASSERT_DEV(instr, "nullptr in bb");
            ASSERT_DEV(instr->bb(), "nullptr in instr bb");
            if (instr->m_type == IR::GroupType::CHECK) {
                checks.push_back(instr);
            }
        }
    }

    if (checks.empty()) {
        return;
    }

    auto currentCheckIt = ++checks.begin();
    auto prevCheckIt = checks.begin();

    while (prevCheckIt != currentCheckIt && currentCheckIt != checks.end()) {
        auto result = dominativeCheckAnalysis(*prevCheckIt, *currentCheckIt, g);
        if (result) {
            auto *i = *currentCheckIt;
            auto *basicBlock = i->bb();
            auto result = basicBlock->erase_instr(i->get_id());
            ASSERT_DEV(result != IR::ID_UNDEF, "Error during instr deletion");
            currentCheckIt = checks.erase(currentCheckIt);
            prevCheckIt = checks.begin();
        } else {
            ++prevCheckIt;
            if (prevCheckIt == currentCheckIt) {
                ++currentCheckIt;
                prevCheckIt = checks.begin();
            }
        }
    }
}