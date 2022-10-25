#include "instruction.hpp"
#include "config.hpp"
#include "exception"
#include <sstream>
namespace IR {

id_t InstrBase::g_id = 1;

InstrBase::InstrBase(const BasicBlock &bb, OpcdT opcode, InstrT type)
    : m_opcd(opcode), m_type(type), m_id(g_id++), m_bb(&bb) {}

InstrBase::~InstrBase() {
    for (auto elem : m_users) {
        delete elem;
    }
}

InstrBase::InstrBase(const InstrBase &other)
    : m_opcd(other.m_opcd), m_type(other.m_type), m_id(g_id++), m_bb(other.m_bb),
      m_prev(other.m_prev), m_next(other.m_next) {
    for (auto it = m_users.begin(); it != m_users.end(); it++) {
        InstrInput *ptr{nullptr};
        if ((*it)->is_phi()) {
            ptr = new PhiInstr(*static_cast<PhiInstr *>(*it));
        } else {
            ptr = new InstrInput(**it);
        }
        m_users.push_back(ptr);
    }
}
InstrBase::InstrBase(const InstrBase &&other)
    : m_opcd(other.m_opcd), m_type(other.m_type), m_id(g_id), m_bb(other.m_bb),
      m_prev(other.m_prev), m_next(other.m_next) {
    for (auto it = m_users.begin(); it != m_users.end(); it++) {
        InstrInput *ptr{nullptr};
        if ((*it)->is_phi()) {
            ptr = new PhiInstr(std::move(*static_cast<PhiInstr *>(*it)));
        } else {
            ptr = new InstrInput(std::move(**it));
        }
        m_users.push_back(ptr);
        *it = nullptr;
    }
}

void InstrBase::set_bb(const BasicBlock *bb) {
    if (bb == nullptr) {
        throw std::logic_error("Invalid bb ptr");
    }
    m_bb = bb;
}

void InstrBase::push_input(const InstrInput &&elem) {
    InstrInput *ptr = new InstrInput(elem);
    m_users.push_back(ptr);
}

void InstrBase::erase_input(CInputListIt cit) {
    if (cit != m_users.cend()) {
        delete *cit;
    }
    m_users.erase(cit);
}

std::string InstrBase::dump() const {
    std::stringstream ss{};
    ss << m_id << ": " << static_cast<int>(m_opcd) << " --> " << static_cast<int>(m_type)
       << std::endl;
    return ss.str();
}

std::string InstrInput::dump() const {
    std::stringstream ss{};
    std::string flow{"???"};
    if (m_flow == FlowT::DEF) {
        flow = "def";
    } else if (m_flow == FlowT::USE) {
        flow = "use";
    }
    ss << "InId: " << m_instr->get_id() << "(" << flow << ")";
    return ss.str();
}

// dump inputs

std::string PhiInstr::dump() const {
    std::stringstream ss{};
    ss << "PHI( ";
    for (auto &elem : m_others) {
        ss << elem.dump();
        ss << "\t";
    }
    ss << ")";
    return ss.str();
}
// UnaryOp::UnaryOp(opcode_t opcode) : Instr(opcode, TYP_UNARY) {}
// BinaryOp::BinaryOp(opcode_t opcode) : Instr(opcode, TYP_BINARY) {}
// CastOp::CastOp(opcode_t opcode) : Instr(opcode, TYP_CAST) {}
// MemOp::MemOp(opcode_t opcode) : Instr(opcode, TYP_MEM) {}
// BranchOp::BranchOp(opcode_t opcode) : Instr(opcode, TYP_BRANCH) {}

} // namespace IR