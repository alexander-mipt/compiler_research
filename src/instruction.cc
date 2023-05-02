#include "basicblock.hpp"
#include "config.hpp"
#include "exception"
#include <algorithm>
#include <set>
#include <sstream>
namespace IR {

/* Instr Base class */
InstrBase::~InstrBase() {}
InstrBase::InstrBase(id_t id) : m_id(id) {}
InstrBase::InstrBase(BasicBlock &bb, id_t id) : m_bb(&bb), m_id(id) {}

void InstrBase::set_prev(InstrBase *instr) { m_prev = instr; }
void InstrBase::set_next(InstrBase *instr) { m_next = instr; }
void InstrBase::set_bb(BasicBlock *bb) { m_bb = bb; }
void InstrBase::set_id(id_t id) { m_id = id; }
std::string InstrBase::dump() const { return "???"; };
InstrBase *InstrBase::prev() const { return m_prev; }
InstrBase *InstrBase::next() const { return m_next; }

InstrBase::UserListIt InstrBase::users_begin() { return m_users.begin(); }
InstrBase::UserListIt InstrBase::users_end() { return m_users.end(); }
InstrBase::UserListIt InstrBase::users_last() {
    if (m_users.size() == 0) {
        return m_users.end();
    }
    return --m_users.end();
}

InstrBase::InputListIt InstrBase::inputs_begin() { return m_inputs.begin(); }
InstrBase::InputListIt InstrBase::inputs_end() { return m_inputs.end(); }
InstrBase::InputListIt InstrBase::inputs_last() {
    if (m_users.size() == 0) {
        return m_inputs.end();
    }
    return --m_inputs.end();
}

id_t InstrBase::get_id() const { return m_id; }

BasicBlock *InstrBase::bb() const { return m_bb; }

void InstrBase::forget_dependencies() {
    m_prev = nullptr;
    m_next = nullptr;
    m_bb = nullptr;
    m_users.clear();
    m_inputs.clear();
}

void InstrBase::push_user(InstrBase *elem) {
    if (!elem) {
        throw std::logic_error("nullptr user");
    }
    // bool duplicate = (std::find(m_users.begin(), m_users.end(), elem) != m_users.end());

    // if (duplicate) {
    //     throw std::logic_error("Instr has already had this user");
    // }
    // duplicate =
    //     (std::find(elem->m_inputs.begin(), elem->m_inputs.end(), this) != elem->m_inputs.end());

    // if (duplicate) {
    //     throw std::logic_error("User has already had this instruction");
    // }

    elem->m_inputs.push_back(this);
    m_users.push_back(elem);
}

// InstrBase::UserListIt InstrBase::erase_user(UserListIt cit) { return m_users.erase(cit); }

bool InstrBase::erase_user(id_t bb_id, id_t id) {
    InputListIt it = m_users.begin();
    bool edited{false};
    while (it != m_users.end()) {
        auto *user = *it;
        ASSERT_DEV(user->bb(), "nullptr bb");
        if (user->bb()->get_id() == bb_id && user->get_id() == id) {
            user->m_inputs.remove(this);
            it = m_users.erase(it);
            edited = true;
        } else {
            ++it;
        }
    }
    return edited;
}

void InstrBase::push_users(initList list) {
    for (auto elem : list) {
        if (elem == nullptr) {
            throw std::logic_error("nullptr in argList");
        }
    }
    for (auto elem : list) {
        push_user(elem);
    }
}

void InstrBase::push_input(InstrBase *elem) {

    if (elem == nullptr) {
        throw std::logic_error("nullptr pushed");
    }

    // bool duplicate = (std::find(m_inputs.begin(), m_inputs.end(), elem) != m_inputs.end());
    // if (duplicate) {
    //     throw std::logic_error("Instr has already had this input");
    // }
    // duplicate =
    //     (std::find(elem->m_users.begin(), elem->m_users.end(), this) != elem->m_users.end());

    // if (duplicate) {
    //     throw std::logic_error("Input has already had this instruction");
    // }

    elem->m_users.push_back(this);
    m_inputs.push_back(elem);
}

// InstrBase::InputListIt InstrBase::erase_input(InputListIt it) { return m_inputs.erase(it); }

bool InstrBase::erase_input(id_t bb_id, id_t id) {
    InputListIt it = m_inputs.begin();
    bool edited{false};
    while (it != m_inputs.end()) {
        auto *input = *it;
        ASSERT_DEV(input->bb(), "nullptr bb");
        if (input->bb()->get_id() == bb_id && input->get_id() == id) {
            input->m_users.remove(this);
            it = m_inputs.erase(it);
            edited = true;
        } else {
            ++it;
        }
    }
    return edited;
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

bool InstrBase::onlyOneInput() const {
    std::set<key_t> keys{};
    for (auto i : m_inputs) {
        keys.insert(i->get_id());
    }
    return (keys.size() == 1);
}

bool InstrBase::onlyOneUser() const {
    std::set<key_t> keys{};
    for (auto i : m_users) {
        keys.insert(i->get_id());
    }
    return (keys.size() == 1);
}

InstrBase::UsersT &InstrBase::users() & { return m_users; }
InstrBase::InputsT &InstrBase::inputs() & { return m_inputs; }

void InstrBase::throwIfWrongDeps_() const {
    if (!m_bb) {
        throw std::logic_error("Instr without bb");
    }
    // analyze inputs
    for (auto input : m_inputs) {
        if (!input) {
            throw std::logic_error("nullptr in inputs");
        }
        if (!input->bb()) {
            throw std::logic_error("input has no bb");
        }
        // check dfg
        bool correct_input_dep =
            (std::find(input->m_users.begin(), input->m_users.end(), this) != input->m_users.end());
        if (!correct_input_dep) {
            throw std::logic_error("Wrong input dependency");
        }
    }
    // analyze users
    for (auto user : m_users) {
        if (!user) {
            throw std::logic_error("nullptr in users");
        }
        if (!user->bb()) {
            throw std::logic_error("user has no bb");
        }
        // check dfg
        bool correct_user_dep =
            (std::find(user->m_inputs.begin(), user->m_inputs.end(), this) != user->m_inputs.end());
        if (!correct_user_dep) {
            throw std::logic_error("Wrong input dependency");
        }
    }

    // check cfg
    if (m_prev && m_prev->next() != this) {
        throw std::logic_error("Wrong prev dependence");
    }
    if (m_next && m_next->prev() != this) {
        throw std::logic_error("Wrong next dependence");
    }
}

void InstrBase::throwIfNotConsistent_() const {
    if (m_id == ID_UNDEF) {
        throw std::logic_error("instr w/ wring id");
    }
}

/* Instr class */
Instr::Instr() {}
Instr::Instr(id_t id, OpcodeType opcode, GroupType type, ValueHolder value)
    : InstrBase(id), m_opcd(opcode), m_type(type), m_value(value) {
    if (value.m_regNum < NO_VALUEHOLDER) {
        throw std::logic_error("Invalid value holder");
    }
}

void Instr::throwIfWrongInputCount_() const {
    switch (m_opcd) {
    case OpcodeType::ADD:
    case OpcodeType::AND:
    case OpcodeType::XOR:
    case OpcodeType::SHL:
        if (m_inputs.size() != 2) {
            throw std::logic_error("Wrong input count");
        }
        break;
    case OpcodeType::MOVI:
    case OpcodeType::CHECK1:
    case OpcodeType::CHECK2:
        if (m_inputs.size() != 1) {
            throw std::logic_error("Wrong input count");
        }
        break;
    case OpcodeType::ICONST:
        if (!m_inputs.empty()) {
            throw std::logic_error("Wrong input count");
        }
        break;
    default:
        break;
    }
}

std::string Instr::dump() const {
    std::stringstream ss{};
    ss $int(m_id) $__ $int(m_opcd) $__ $int(m_type) $__ $int(m_value.m_regNum)
        $__ $int(m_value.m_val) $__;
    if (m_bb != nullptr) {
        auto bb_id = m_bb->get_id();
        ss $int(bb_id) $__;
    }
    if (m_prev != nullptr) {
        auto prev = m_prev->get_id();
        ss $int(prev) $__;
    }
    if (m_next != nullptr) {
        auto next = m_next->get_id();
        ss $int(next) $__;
    }
    ss $("I: ");
    for (auto ptr : m_inputs) {
        ASSERT_DEV(ptr, "Nullptr input");
        if (ptr != nullptr) {
            ss << ptr->get_id() << " ";
        }
    }
    ss $__;
    ss $("U: ");
    for (auto ptr : m_users) {
        ASSERT_DEV(ptr, "Nullptr user");
        if (ptr != nullptr) {
            ss << ptr->get_id() << " ";
        }
    }
    return ss.str();
}

/* Phy class */
Phy::Phy() : InstrBase() {}
Phy::Phy(id_t id) : InstrBase(id) {}

std::string Phy::dump() const {
    std::stringstream ss{};
    ss << "PHY( ";
    for (const auto *elem : m_users) {
        ss << elem->bb()->get_id() << ":" << elem->get_id();

        ss << "; ";
    }
    ss << ")";
    return ss.str();
}

} // namespace IR