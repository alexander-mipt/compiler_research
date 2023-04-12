#include "instruction.hpp"
#include "basicblock.hpp"
#include <exception>
#include <vector>
#include <set>

namespace IR {

template <size_t capacity = 20> class InstrManager {
  public:
    InstrManager(const InstrManager &) = delete;
    InstrManager(InstrManager &&) = delete;
    InstrManager() { m_mem.reserve(capacity); }
    ~InstrManager() {
        for (auto i : m_mem) {
            delete i;
        }
    }

    Instr *create(InstrT type, GroupT group, id_t id = -1) {
        if (group == GroupT::PHY || type == TypeT::PHY) {
            throw std::logic_error("Wrong instr type");
        }

        id_t id0 = id == -1? m_id : id;
        
        InstrBase *ptr = new Instr(type, group, id0);
        
        m_id = std::max(m_id, id0) + 1;
        m_mem.push_back(ptr);

        return ptr;
    }

    Phy *create(id_t id = -1) {
        id_t id0 = id == -1? m_id : id;
        
        InstrBase *ptr = new Phy(id0);
        
        m_id = std::max(m_id, id0) + 1;
        m_mem.push_back(ptr);

        return ptr;
    }

    void throwIfNotConsistent_() const {
        std::set<id_t> ids{};
        for (const auto *i: m_mem) {
            if (!i) {
                throw std::logic_error("instr w/ nullptr");
            }
            i->throwIfNotConsistent_();

        }
        if (ids.size() != m_mem.size()) {
            throw std::logic_error("instr ids collision");
        }
    }

  private:
    std::vector<InstrBase *> m_mem{};
    id_t m_id{ID_UNDEF};
};

class BasicBlockManager {
    BasicBlockManager();
    ~BasicBlockManager() {
        for (auto i : m_mem) {
            delete i;
        }
    }

    BasicBlock *create(id_t id = -1) {
        id_t id0 = id == -1? m_id : id;
        
        BasicBlock *ptr = new BasicBlock(id0);
        
        m_id = std::max(m_id, id0) + 1;
        m_mem.push_back(ptr);

        return ptr;
    }

    void throwIfNotConsistent_() const {
        std::set<id_t> ids{};
        for (const auto *i: m_mem) {
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
    id_t m_id{ID_UNDEF};
};


} // namespace IR