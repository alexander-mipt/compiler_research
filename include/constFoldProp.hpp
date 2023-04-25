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
// I1 = I0 + C0
// I2 = I1 + C1
bool makePeepholes(IR::Instr *i, IR::BbGraph &g, IR::InstrManager &mem) {
    ASSERT_DEV(i->inputs().size() == 2, "Wrong instr inputs");
    auto input1 = i->inputs().front();
    auto input2 = i->inputs().back();
    if (input1->type() == IR::GroupType::PHY || input2->type() == IR::GroupType::PHY) {
        return false;
    }
    auto *arg1 = static_cast<IR::Instr *>(input1);
    auto *arg2 = static_cast<IR::Instr *>(input2);
    if (arg1->m_opcd != i->m_opcd ||
        (i->m_opcd != IR::OpcodeType::ADD && i->m_opcd != IR::OpcodeType::AND)) {
        return false;
    }
    if ((arg1->type() == IR::GroupType::CONST && arg2->type() == IR::GroupType::CONST) ||
        (arg1->type() != IR::GroupType::CONST && arg2->type() != IR::GroupType::CONST)) {
        return false;
    }
    if ((arg1->type() != IR::GroupType::CONST && arg2->type() == IR::GroupType::CONST)) {
        if (!arg1->onlyOneUser()) {
            return false;
        }

        auto prevArgs = detachInputsFromArg(arg1);
        // peephole can not be resolved
        if (!prevArgs.first) {
            return false;
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
        return true;
    }
    return false;
}

bool doPeepholes(IR::BbGraph &g, IR::InstrManager &mem) {
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
        auto replace = makePeepholes(instr, g, mem);
        // if (replace) {
        //     bb.removeUnusedInstrs();
        // }
        ++it;
    }
    return true;
}