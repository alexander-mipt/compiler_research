#pragma once
#include "config.hpp"
#include "types.hpp"
#include "instruction.hpp"
#include <list>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace IR {

class BasicBlock final {
    using InstrCIt = typename std::list<Instr *>::const_iterator;
    using PhyCIt = typename std::list<Phy *>::const_iterator;
    using InstrIt = typename std::list<Instr *>::iterator;
    using PhyIt = typename std::list<Phy *>::iterator;
    using InstrContainer = std::list<Instr *>;
    using PhyContainer = std::list<Phy *>;
    using InstrInitList = std::initializer_list<const Instr*>;
    using PhyInitList = std::initializer_list<const Phy*>;

  public:
    BasicBlock();
    BasicBlock(id_t id);
    // copy instrs by ptr & create internal CF connections
    BasicBlock(id_t id, InstrInitList instrs, PhyInitList phys);
    // creates copy w/ same DF dependencies, but CF, id and bb id are edited 
    void push_instrs(InstrInitList instrs);

    BasicBlock(const BasicBlock &&bb) = delete;
    BasicBlock &operator=(const BasicBlock &&) = delete;
    BasicBlock(const BasicBlock &bb) = delete;
    BasicBlock &operator=(const BasicBlock &) = delete;
    ~BasicBlock();

    void throwIfNonConsistence_() const;

    id_t set_id(id_t id) {
        if (id == ID_UNDEF) {
            ERROR("Wrong Bb id");
            return ID_UNDEF;
        }
        if (m_bb_id != ID_UNDEF) {
            ERROR("Bb id has been already inited");
            return ID_UNDEF;
        }
        m_bb_id = id;
    }

    PhyCIt push_phys(PhyInitList list) {
        PhiInstr *ptr = new PhiInstr(phi);
        m_phi_ptrs.push_back(ptr);
    }
    void pop_instr() {
        if (m_instr_ptrs.size() > 0) {
            InstrBase *instr = *(m_instr_ptrs.rbegin());
            delete instr;
            m_instr_ptrs.pop_back();
        }
    }
    void pop_phi() {
        if (m_phi_ptrs.size() > 0) {
            PhiInstr *phi = *(m_phi_ptrs.rbegin());
            delete phi;
            m_phi_ptrs.pop_back();
        }
    }

    InstrIt instr_begin();
    InstrIt instr_end();
    PhyIt phy_begin();
    PhyIt phy_end();

    std::vector<std::string> dump() const;
    id_t get_id() const { return m_bb_id; }
    InstrCIt instr_cbegin() const;
    InstrCIt instr_cend() const;
    PhyCIt phy_cbegin() const;
    PhyCIt phy_cend() const;

  private:
    id_t m_bb_id{ID_UNDEF};
    PhyContainer m_phys{};
    InstrContainer m_instrs{};
    id_t m_cur_instr_id{ID_UNDEF};
};
} // namespace IR
