#pragma once
#include "basicblock.hpp"
#include "instruction.hpp"
#include <exception>
#include <set>
#include <vector>

namespace IR {

class InstrManager {
  public:
    InstrManager(const InstrManager &) = delete;
    InstrManager(InstrManager &&) = delete;
    InstrManager() = default;
    void print() {
        for (auto *i : m_mem) {
            std::cout << i->get_id() << std::endl;
        }
    }
    ~InstrManager() {
        for (auto i : m_mem) {
            delete i;
        }
    }

    Instr *createADD(ValueHolder value) {
        return create(OpcodeType::ADD, GroupType::GENERAL64, value);
    }
    Instr *createXOR(ValueHolder value) {
        return create(OpcodeType::XOR, GroupType::GENERAL64, value);
    }
    Instr *createSHL(ValueHolder value) {
        return create(OpcodeType::SHL, GroupType::GENERAL64, value);
    }
    Instr *createCONST(ValueHolder value) {
        if (value.m_regNum == NO_VALUEHOLDER) {
            return create(OpcodeType::ICONST, GroupType::CONST, value);
        }
        return create(OpcodeType::MOVI, GroupType::CONST, value);
    }

    Instr *createCheck(OpcodeType opcode) {
        if (opcode != OpcodeType::CHECK1 && opcode != OpcodeType::CHECK2) {
            return nullptr;
        }
        return create(opcode, GroupType::CHECK, {});
    }

    Instr *create(OpcodeType type, GroupType group, ValueHolder value, id_t id = -1) {
        if (group == GroupType::PHY || type == OpcodeType::PHY) {
            throw std::logic_error("Wrong instr type");
        }

        id_t id0 = (id == -1) ? m_id : id;

        Instr *ptr = new Instr(id0, type, group, value);

        m_id = std::max(m_id, id0) + 1;
        m_mem.push_back(ptr);

        return ptr;
    }

    Phy *create(id_t id = -1) {
        id_t id0 = id == -1 ? m_id : id;

        Phy *ptr = new Phy(id0);

        m_id = std::max(m_id, id0) + 1;
        m_mem.push_back(ptr);

        return ptr;
    }

    void throwIfNotConsistent_() const {
        std::set<id_t> ids{};
        for (const auto *i : m_mem) {
            if (!i) {
                throw std::logic_error("instr w/ nullptr");
            }
            i->throwIfNotConsistent_();
            ids.insert(i->get_id());
        }
        if (ids.size() != m_mem.size()) {
            throw std::logic_error("instr ids collision");
        }
    }

  private:
    std::vector<InstrBase *> m_mem{};
    id_t m_id{1};
};

class BasicBlockManager {
  public:
    BasicBlockManager() = default;
    ~BasicBlockManager() {
        for (auto i : m_mem) {
            delete i;
        }
    }

    BasicBlock *create(id_t id = -1) {
        id_t id0 = id == -1 ? m_id : id;

        BasicBlock *ptr = new BasicBlock(id0);

        m_id = std::max(m_id, id0) + 1;
        m_mem.push_back(ptr);

        return ptr;
    }

    void throwIfNotConsistent_() const {
        std::set<id_t> ids{};
        for (const auto *i : m_mem) {
            if (!i) {
                throw std::logic_error("basic block w/ nullptr");
            }
            i->throwIfNotConsistent_();
        }
        if (ids.size() != m_mem.size()) {
            throw std::logic_error("basic block ids collision");
        }
    }

  private:
    std::vector<BasicBlock *> m_mem{};
    id_t m_id{1};
};

} // namespace IR