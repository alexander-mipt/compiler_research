#include "basicblock.hpp"
#include "config.hpp"
#include "exception"
#include <sstream>
namespace IR {

/* Instr Base class */
InstrBase::~InstrBase() {}
InstrBase::InstrBase(const BasicBlock &bb) : m_bb(&bb) {}
InstrBase::InstrBase(const BasicBlock &bb, id_t id) : m_bb(&bb), m_id(id) {}
InstrBase::InstrBase(const BasicBlock &bb, id_t id, initList list) : InstrBase(bb, id) {
    for (const auto* ptr: list) {
        if (ptr == nullptr) {
            throw std::logic_error("nullptr arg");
        }
    }
    for (const auto* ptr: list) {
        m_users.push_back(ptr);
    }
}
void InstrBase::set_prev(const InstrBase *instr) { m_prev = instr; }
void InstrBase::set_next(const InstrBase *instr) { m_next = instr; }
void InstrBase::set_bb(const BasicBlock *bb) { m_bb = bb; }
void InstrBase::set_id(id_t id) { m_id = id; }
std::string InstrBase::dump() const { return "???"; };
InstrT InstrBase::get_type() const { return InstrT::UNDEF; };
const InstrBase *InstrBase::get_prev() const { return m_prev; }
const InstrBase *InstrBase::get_next() const { return m_next; }
InstrBase::CInputListIt InstrBase::cbegin() const { return m_users.cbegin(); }
InstrBase::CInputListIt InstrBase::cend() const { return m_users.cend(); }
InstrBase::CInputListIt InstrBase::clast() const {
    if (m_users.size() == 0) {
        return m_users.cend();
    }
    return --m_users.cend();
}
InstrBase::InputListIt InstrBase::begin() { return m_users.begin(); }
InstrBase::InputListIt InstrBase::end() { return m_users.end(); }
InstrBase::InputListIt InstrBase::last() {
    if (m_users.size() == 0) {
        return m_users.end();
    }
    return --m_users.end();
}
id_t InstrBase::get_id() const { return m_id; }

const BasicBlock &InstrBase::get_bb() const {
    return *m_bb;
}

void InstrBase::forget_dependencies() {
    m_prev = nullptr;
    m_next = nullptr;
    m_bb = nullptr;
    m_users.clear();
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

InstrBase::InputListIt InstrBase::erase_input(InputListIt cit) { return m_users.erase(cit); }

bool InstrBase::erase_input(id_t bb_id, id_t id) {
    for (InputListIt it = m_users.begin(); it != m_users.end(); ++it) {
        const auto *input = *it;
        if (input->get_bb().get_id() == bb_id && input->get_id() == id) {
            InputListIt res = m_users.erase(it);
            return (res != it);
        }
    }
    return false;
}

void InstrBase::push_inputs(initList list) {
    for (auto elem : list) {
        if (elem == nullptr) {
            throw std::logic_error("nullptr in argList");
        }
    }
    for (auto elem : list) {
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
Instr::Instr(const BasicBlock &bb, id_t id, OpcdT opcode, InstrT type)
    : InstrBase(bb, id), m_opcd(opcode), m_type(type) {}

std::string Instr::dump() const {
    std::stringstream ss{};
    ss $intn(m_id) $intn(m_opcd) $intn(m_type);
    if (m_bb != nullptr) {
        auto bb_id = m_bb->get_id();
        ss $intn(bb_id);
    }
    if (m_next != nullptr) {
        auto next = m_next->get_id();
        ss $intn(next);
    }
    if (m_prev != nullptr) {
        auto prev = m_prev->get_id();
        ss $intn(prev);
    }
    ss $("Users: ");
    for (auto ptr : m_users) {
        if (ptr != nullptr) {
            ss << ptr->get_id() << " ";
        }
    }
    ss $n("");
    return ss.str();
}

/* Phy class */
Phy::Phy() : InstrBase() {}
Phy::Phy(const BasicBlock &bb, id_t id, initList list) : InstrBase(bb, id, list) {}


std::string Phy::dump() const {
    std::stringstream ss{};
    ss << "PHY( ";
    for (const auto *elem : m_users) {
        ss << elem->get_bb().get_id() << ":" << elem->get_id();

        ss << "; ";
    }
    ss << ")";
    return ss.str();
}

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