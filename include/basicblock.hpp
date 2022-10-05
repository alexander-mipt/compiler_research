#pragma once
#include "instruction.hpp"
#include <utility>
#include <list>
#include <string>
#include <sstream>
#include "config.hpp"
#include "types.hpp"

namespace IR {

class BasicBlock {
    public:
    BasicBlock();
    BasicBlock(const BasicBlock& bb);
    BasicBlock(const BasicBlock&& bb) = delete;
    ~BasicBlock();
    void push_phi(const PHI& phi);
    template <typename Instr_T>
    void push_instr(const Instr_T& instr);
    void pop_instr();
    void pop_phi();
    std::string dump() const;    
    private:
    id_t m_bb_id{ID_UNDEF};
    Instr *m_first{nullptr};
    Instr *m_last{nullptr};
    PHI* m_first_phi{nullptr};
    std::list<Instr*> m_instr_ptrs{};
    std::list<PHI*> m_phi_ptrs{};
};

template <typename Instr_T>
void BasicBlock::push_instr(const Instr_T &instr) {
    Instr* prev_instr{nullptr};
    if (m_instr_ptrs.size() > 0) {
        prev_instr = m_instr_ptrs.back();
    }

    m_instr_ptrs.push_back(new Instr_T{instr});
    m_first = m_instr_ptrs.front();
    m_last = m_instr_ptrs.back();

    m_last->m_prev = prev_instr;

    if (m_instr_ptrs.size() > 1) {
        prev_instr->m_next = m_last;
    }

}
}
