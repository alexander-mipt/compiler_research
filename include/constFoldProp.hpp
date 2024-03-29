#pragma once
#include "bbGraph.hpp"
#include "instruction.hpp"
#include "memManager.hpp"

using InstrIt = IR::BasicBlock::InstrIt;

bool calcConstants(IR::Instr *i) {
    ASSERT_DEV(i->inputs().size() == 2, "Wrong instr inputs");
    auto input1 = i->inputs().front();
    auto input2 = i->inputs().back();
    if (input1->type() == IR::GroupType::PHY || input2->type() == IR::GroupType::PHY) {
        return false;
    }
    auto *arg1 = static_cast<IR::Instr *>(input1);
    auto *arg2 = static_cast<IR::Instr *>(input2);
    if (arg1->type() == IR::GroupType::CONST && arg2->type() == IR::GroupType::CONST) {
        switch (i->m_opcd) {
        case IR::OpcodeType::ADD:
            i->m_value.m_val = arg1->m_value.m_val + arg2->m_value.m_val;
            break;
        case IR::OpcodeType::AND:
            i->m_value.m_val = arg1->m_value.m_val & arg2->m_value.m_val;
            break;
        case IR::OpcodeType::XOR:
            i->m_value.m_val = arg1->m_value.m_val ^ arg2->m_value.m_val;
            break;
        }
        i->m_type = IR::GroupType::CONST;
        return true;
    }
    return false;
}

// calculate all conostants and move all middle or end results in head as constants
bool doConstFoldProp(IR::BbGraph &g) {
    if (g.get_node_count() < 1) {
        return false;
    }
    // first bb - constants and inputs
    auto BbIt = ++g.nodes_begin();
    auto &bb = BbIt->second->data();
    auto *head = g.accessHeader();
    ASSERT_DEV(head, "No head detected");
    IR::BasicBlock::InstrIt it = bb.instr_begin();
    while (it != bb.instr_end()) {
        auto instr = *it;
        auto replace = calcConstants(instr);
        if (replace) {
            auto cut = bb.cut_instr(instr->get_id());
            ASSERT_DEV(cut.second, "Instr does not exist");
            head->data().push_instrs({instr});
            it = cut.first;
        } else {
            ++it;
        }
    }
    return true;
}

std::pair<IR::Instr *, IR::Instr *> detachInputsFromArg(IR::Instr *arg) {
    ASSERT_DEV(arg->inputs().size() == 2, "Wrong count of inputs for this peephole");
    auto input1 = arg->inputs().front();
    auto input2 = arg->inputs().back();
    if (input1->type() == IR::GroupType::PHY || input2->type() == IR::GroupType::PHY) {
        return {nullptr, nullptr};
    }
    auto *arg1 = static_cast<IR::Instr *>(input1);
    auto *arg2 = static_cast<IR::Instr *>(input2);
    if ((arg1->type() != IR::GroupType::CONST && arg2->type() == IR::GroupType::CONST) ||
        (arg1->type() == IR::GroupType::CONST && arg2->type() != IR::GroupType::CONST)) {

        arg->erase_input(arg1->bb()->get_id(), arg1->get_id());
        arg->erase_input(arg2->bb()->get_id(), arg2->get_id());
        if (arg1->type() != IR::GroupType::CONST) {
            return {arg1, arg2};
        }
        return {arg2, arg1};
    }
    return {nullptr, nullptr};
}

// peephole:
// I1 = I0 + C0
// I2 = I1 + C1
// --> I2 = I0 + C2
id_t makePeepholes(IR::Instr *i, IR::BbGraph &g, IR::InstrManager &mem) {
    if (i->m_opcd != IR::OpcodeType::ADD && i->m_opcd != IR::OpcodeType::XOR) {
        return IR::ID_UNDEF;
    }
    ASSERT_DEV(i->inputs().size() == 2, "Wrong instr inputs");
    auto input1 = i->inputs().front();
    auto input2 = i->inputs().back();
    if (input1->type() == IR::GroupType::PHY || input2->type() == IR::GroupType::PHY) {
        return IR::ID_UNDEF;
    }
    auto *arg1 = static_cast<IR::Instr *>(input1);
    auto *arg2 = static_cast<IR::Instr *>(input2);
    if (arg1->m_opcd != i->m_opcd ||
        (i->m_opcd != IR::OpcodeType::ADD && i->m_opcd != IR::OpcodeType::AND)) {
        return IR::ID_UNDEF;
    }
    if ((arg1->type() == IR::GroupType::CONST && arg2->type() == IR::GroupType::CONST) ||
        (arg1->type() != IR::GroupType::CONST && arg2->type() != IR::GroupType::CONST)) {
        return IR::ID_UNDEF;
    }
    if ((arg1->type() != IR::GroupType::CONST && arg2->type() == IR::GroupType::CONST)) {
        if (!arg1->onlyOneUser()) {
            return IR::ID_UNDEF;
        }

        auto prevArgs = detachInputsFromArg(arg1);
        // peephole can not be resolved
        if (!prevArgs.first) {
            return IR::ID_UNDEF;
        }
        // now I0, C0 are not inputs for I1
        // delete I1, C1 as inputs for I2
        i->erase_input(arg1->bb()->get_id(), arg1->get_id());
        i->erase_input(arg2->bb()->get_id(), arg2->get_id());
        // add I0 as input for I2
        i->push_input(prevArgs.first);
        // create and init new C3 = F(C1,C2)
        auto val{0};
        switch (i->m_opcd) {
        case IR::OpcodeType::ADD:
            val = arg2->m_value.m_val + prevArgs.second->m_value.m_val;
            break;
        case IR::OpcodeType::AND:
            val = arg2->m_value.m_val & prevArgs.second->m_value.m_val;
            break;
        }
        auto *c3 = mem.createCONST(IR::ValueHolder(val, IR::NO_VALUEHOLDER));
        g.accessHeader()->data().push_instrs({c3});
        // make as I2 input
        i->push_input(c3);
        return arg1->get_id();
    }
    return IR::ID_UNDEF;
}

bool doPeepholes(IR::BbGraph &g, IR::InstrManager &mem) {
    if (g.get_node_count() < 1) {
        return false;
    }
    // first bb - constants and inputs
    auto *head = g.accessHeader();
    ASSERT_DEV(head, "No head detected");
    auto rpo = g.RPO(head->get_key());
    auto keyIt = ++rpo.begin();
    while (keyIt != rpo.end()) {
        auto &bb = g.at(*keyIt)->data();
        IR::BasicBlock::InstrIt it = bb.instr_begin();
        std::vector<IR::id_t> oddInstrs{};
        while (it != bb.instr_end()) {
            auto instr = *it;
            auto replace = makePeepholes(instr, g, mem);
            if (replace != IR::ID_UNDEF) {
                oddInstrs.push_back(replace);
            }
            ++it;
        }
        while(!oddInstrs.empty()) {
            auto id = oddInstrs.back();
            std::cout << "delete " << id << std::endl;
            bb.erase_unused_instr(id);
            oddInstrs.pop_back();
        }
        ++keyIt;
    }
    g.removeUnusedConsts();
    return true;
}

bool applyPeephole(IR::Instr *i, IR::InstrManager &mem, IR::BasicBlock &head) {
    // XOR a, a --> 0
    if (i->m_opcd == IR::OpcodeType::XOR && i->inputs().front() == i->inputs().back()) {
        i->m_type = IR::GroupType::CONST;
        i->m_value.m_val =  0;
        i->m_value.m_regNum = IR::NO_VALUEHOLDER;
        return true; // move to head as const
    }
    // AND a, 0 --> 0
    if (i->m_opcd == IR::OpcodeType::AND) {
        auto *arg1 = i->inputs().front();
        auto *arg2 = i->inputs().back();
        if (arg1->type() == IR::GroupType::CONST) {
            auto *arg = static_cast<IR::Instr*>(arg1);
            if (arg->m_value.m_val == 0) {
                i->m_type = IR::GroupType::CONST;
                i->m_value.m_val = 0;
                i->m_value.m_regNum = IR::NO_VALUEHOLDER;
                return true; // move to head as const
            }
        } else if (arg2->type() == IR::GroupType::CONST) {
            auto *arg = static_cast<IR::Instr*>(arg2);
            if (arg->m_value.m_val == 0) {
                i->m_type = IR::GroupType::CONST;
                i->m_value.m_val = 0;
                i->m_value.m_regNum = IR::NO_VALUEHOLDER;
                return true; // move to head as const
            }
        }
    }
    // add a, a --> shl a, 1
    if (i->m_opcd == IR::OpcodeType::ADD && i->inputs().front() == i->inputs().back()) {
        i->m_opcd = IR::OpcodeType::SHL;
        auto *arg = i->inputs().front();
        i->inputs().remove(arg);
        i->inputs().push_back(arg);
        auto *cnst = mem.createCONST({1, IR::NO_VALUEHOLDER});
        i->push_input(cnst);
        head.push_instrs({cnst});
        return false; // do not move to head as const
    }
    return false;
}

// peepholes that do not have relationship with other instrs
void doAtomicPeepholes(IR::BbGraph &g, IR::InstrManager &mem) {
    if (g.get_node_count() < 1) {
        return;
    }
    // first bb - constants and inputs
    auto *head = g.accessHeader();
    ASSERT_DEV(head, "No head detected");
    auto rpo = g.RPO(head->get_key());
    auto keyIt = ++rpo.begin();
    while (keyIt != rpo.end()) {
        auto &bb = g.at(*keyIt)->data();
        IR::BasicBlock::InstrIt it = bb.instr_begin();
        while (it != bb.instr_end()) {
            auto instr = *it;
            if (applyPeephole(instr, mem, head->data())) {
                auto itAndInstr = bb.cut_instr(instr->get_id());
                head->data().push_instrs({itAndInstr.second});
                it = itAndInstr.first;
            } else {
                ++it;
            }
        }
        ++keyIt;
    }
}