#include "basicblock.hpp"
#include "instruction.hpp"
#include <iostream>
namespace IR {
BasicBlock::BasicBlock(const BasicBlock &bb) {
    for (Instr *ptr : bb.m_instr_ptrs) {
        Instr *base_ptr{nullptr};
        switch (ptr->m_type) {
        case TYP_UNARY:
            base_ptr = new UnaryOp(*(dynamic_cast<UnaryOp *>(ptr)));
            break;
        case TYP_BINARY:
            base_ptr = new BinaryOp(*(dynamic_cast<BinaryOp *>(ptr)));
            break;
        case TYP_CAST:
            base_ptr = new CastOp(*(dynamic_cast<CastOp *>(ptr)));
            break;
        case TYP_MEM:
            base_ptr = new MemOp(*(dynamic_cast<MemOp *>(ptr)));
            break;
        case TYP_BRANCH:
            base_ptr = new BranchOp(*(dynamic_cast<BranchOp *>(ptr)));
            break;
        default:
            base_ptr = new Instr(*ptr);
            break;
        }
        m_instr_ptrs.push_back(base_ptr);
    }
    m_first = m_instr_ptrs.front();
    m_last = m_instr_ptrs.back();

    for (PHI *ptr : bb.m_phi_ptrs) {
        m_phi_ptrs.push_back(new PHI(*ptr));
    }
    m_first_phi = m_phi_ptrs.front();
}

BasicBlock::~BasicBlock() {
    while (m_instr_ptrs.size() > 0) {
        delete m_instr_ptrs.back();
        m_instr_ptrs.pop_back();
    }
    while (m_phi_ptrs.size() > 0) {
        delete m_phi_ptrs.back();
        m_phi_ptrs.pop_back();
    }
}

void BasicBlock::push_phi(const PHI& phi) {
    m_phi_ptrs.push_back(new PHI{phi});
    m_first_phi = m_phi_ptrs.front();
}

void BasicBlock::pop_phi() {
    auto *ptr = m_phi_ptrs.back();
    delete ptr;
    m_phi_ptrs.pop_back();
}


void BasicBlock::pop_instr() {
    auto *ptr = m_instr_ptrs.back();
    delete ptr;
    m_instr_ptrs.pop_back();
}
} // namespace IR