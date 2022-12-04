#include "basicblock.hpp"
#include "config.hpp"
#include "defines.hpp"
#include "exception"
#include <sstream>
namespace IR {

/* Instr Base class */
InstrBase::~InstrBase() {}
InstrBase::InstrBase(const BasicBlock &bb) : m_bb(&bb) {}
void InstrBase::set_prev(const InstrBase *instr) { m_prev = instr; }
void InstrBase::set_next(const InstrBase *instr) { m_next = instr; }
void InstrBase::set_bb(const BasicBlock *bb) { m_bb = bb; }
std::string InstrBase::dump() const { return "???"; };
InstrT InstrBase::get_type() const { return InstrT::UNDEF; };
const InstrBase *InstrBase::get_prev() const { return m_prev; }
const InstrBase *InstrBase::get_next() const { return m_next; }
InstrBase::CInputListIt InstrBase::cbegin() const { return m_users.cbegin(); }
InstrBase::CInputListIt InstrBase::cend() const { return m_users.cend(); }
InstrBase::CInputListIt InstrBase::clast() const { return --m_users.cend(); }
InstrBase::InputListIt InstrBase::begin() { return m_users.begin(); }
InstrBase::InputListIt InstrBase::end() { return m_users.end(); }
InstrBase::InputListIt InstrBase::last() { return --m_users.end(); }
id_t InstrBase::get_id() const { return m_id; }

void InstrBase::set_bb(const BasicBlock *bb) {
    if (bb == nullptr) {
        throw std::logic_error("Invalid bb ptr");
    }
    m_bb = bb;
}

void InstrBase::push_input(const InstrBase *elem) {
    for (auto ptr : m_users) {
        if (ptr == nullptr) {
            throw std::logic_error("nullptr pushed");
        }
        if (elem == ptr) {
            throw std::logic_error("Instr has already has this input");
        }
    }
    m_users.push_back(elem);
}

InstrBase::CInputListIt InstrBase::erase_input(CInputListIt cit) { return m_users.erase(cit); }

void InstrBase::push_inputs(initList list) {
    for (auto elem: list) {
        if (elem == nullptr) {
            throw std::logic_error("nullptr in argList");
        }
    }
    for (auto elem: list) {
        push_input(elem);
    }
}

void InstrBase::throwIfNonConsistence_() const {
    bool fault{0};
    fault |= (m_bb == nullptr);
    fault |= (m_id == ID_UNDEF);
    for (auto ptr : m_users) {
        fault |= (ptr == nullptr);
    }
    if (fault) {
        throw std::logic_error("Inconsistent instr base");
    }
}

/* Instr class */
Instr::Instr() {}
Instr::Instr(const BasicBlock &bb, OpcdT opcode, InstrT type) : InstrBase(bb), m_opcd(opcode), m_type(type) {} 

Instr::Instr(const BasicBlock &bb, OpcdT opcode, InstrT type, initList list) : Instr(bb, opcode, type) {
    push_inputs(list);
}

std::string Instr::dump() const {
    std::stringstream ss{};
    ss $intn(m_id) $intn(m_opcd) $intn(m_type);
    if (m_bb != nullptr) {
        auto bb_id = m_bb->get_id();
        ss $intn(bb_id);
    }
    if (m_next != nullptr ) {
        auto next = m_next->get_id();
        ss $intn(next);
    }
    if (m_prev != nullptr) {
        auto prev = m_prev->get_id();
        ss $intn(prev);
    }
    ss $("Users: ");
    for (auto ptr: m_users) {
        if (ptr != nullptr) {
            ss << ptr->get_id() << " ";
        }
    }
    ss $n("");
    return ss.str();
}

#if 0
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
#endif
#if 0
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
#endif

} // namespace IR