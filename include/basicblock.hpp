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
    using InstrCIt = typename std::list<InstrBase *>::const_iterator;
    using PhiCIt = typename std::list<PhiInstr *>::const_iterator;

  public:
    // BasicBlock();

    BasicBlock(const BasicBlock &&bb);
    BasicBlock &operator=(const BasicBlock &&) = delete;
    BasicBlock(const BasicBlock &bb) = delete;
    BasicBlock &operator=(const BasicBlock &) = delete;
    ~BasicBlock() {
        for (auto ptr : m_instr_ptrs) {
            delete ptr;
        }
        for (auto ptr : m_phi_ptrs) {
            delete ptr;
        }
    }

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

    InstrCIt push_instr(const InstrBase &instr) {
        InstrBase *ptr = new InstrBase(instr);

        m_instr_ptrs.push_back(ptr);
    }
    PhiCIt push_phi(const PhiInstr &phi) {
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

    std::vector<std::string> dump() const;
    id_t get_id() const { return m_bb_id; }
    InstrCIt instr_cbegin() const { return m_instr_ptrs.cbegin(); }
    InstrCIt instr_cend() const { return m_instr_ptrs.cend(); }
    PhiCIt phi_cbegin() const { return m_phi_ptrs.cbegin(); }
    PhiCIt phi_cend() const { return m_phi_ptrs.cend(); }

  private:
    id_t m_bb_id{ID_UNDEF};
    std::list<PhiInstr *> m_phi_ptrs{};
    std::list<InstrBase *> m_instr_ptrs{};
};
} // namespace IR
