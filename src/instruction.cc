#include "config.hpp"
#include "instruction.hpp"

namespace IR {

Instr::Instr(){};
Instr::Instr(opcode_t opcode, instr_t type) : m_opcd(opcode), m_type(type) {}
std::string Instr::dump() const {
    $hex(this);
    $(m_opcd);
    $(m_type);
    $(m_bb);
    $hex(m_next);
    $hex(m_prev);
}

UnaryOp::UnaryOp(opcode_t opcode) : Instr(opcode, TYP_UNARY) {}
BinaryOp::BinaryOp(opcode_t opcode) : Instr(opcode, TYP_BINARY) {}
CastOp::CastOp(opcode_t opcode) : Instr(opcode, TYP_CAST) {}
MemOp::MemOp(opcode_t opcode) : Instr(opcode, TYP_MEM) {}
BranchOp::BranchOp(opcode_t opcode) : Instr(opcode, TYP_BRANCH) {}

} // namespace IR