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

struct InstrArgs {
    OpcdT opcode{OpcdT::UNDEF};
    InstrT type{InstrT::UNDEF};
};

class BasicBlock final {
    using InstrCIt = typename std::list<Instr *>::const_iterator;
    using PhyCIt = typename std::list<Phy *>::const_iterator;
    using InstrIt = typename std::list<Instr *>::iterator;
    using PhyIt = typename std::list<Phy *>::iterator;
    using InstrContainer = std::list<Instr *>;
    using PhyContainer = std::list<Phy *>;
    using InstrInitList = std::initializer_list<InstrArgs>;
    using PhyInitList = std::initializer_list<const Phy*>;

  public:
    BasicBlock();
    BasicBlock(id_t id);
    // copy instrs by ptr & create internal CF connections
    BasicBlock(id_t id, InstrInitList instrs, PhyInitList phys);
    BasicBlock(const BasicBlock &&bb) = delete;
    BasicBlock &operator=(const BasicBlock &&) = delete;
    BasicBlock(const BasicBlock &bb) = delete;
    BasicBlock &operator=(const BasicBlock &) = delete;
    ~BasicBlock();

    // creates copy w/ same DF dependencies, but CF, id and bb id are edited 
    // do not copy DF
    void push_instrs(InstrInitList list);
    void push_phys(PhyInitList list);
    InstrIt erase_instr(InstrIt it);
    PhyIt erase_phy(PhyIt it);
    id_t set_id(id_t id);
    InstrIt instr_begin();
    InstrIt instr_end();
    InstrIt instr_last();
    PhyIt phy_begin();
    PhyIt phy_end();
    PhyIt phy_last();

    void throwIfNonConsistence_() const;
    std::vector<std::string> dump() const;
    id_t get_id() const;
    InstrCIt instr_cbegin() const;
    InstrCIt instr_cend() const;
    InstrCIt instr_clast() const;
    PhyCIt phy_cbegin() const;
    PhyCIt phy_cend() const;
    PhyCIt phy_clast() const;

  private:
    id_t m_bb_id{ID_UNDEF};
    PhyContainer m_phys{};
    InstrContainer m_instrs{};
    id_t m_cur_instr_id{ID_UNDEF};
};
} // namespace IR
