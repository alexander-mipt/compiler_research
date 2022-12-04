#pragma once
#include "types.hpp"
#include <cstdint>
#include <list>
#include <optional>
#include <string>
#include <unistd.h>

namespace IR {

class BasicBlock;
class InstrBase;

using initList = std::initializer_list<const InstrBase *>;

enum class OpcdT {
    NONE,
    CAST,
    ADDI,
    ADD,
    SUBI,
    SUB,
    MULI,
    MUL,
    MOVI,
    MOV,
    CMP,
    JA,
    JAE,
    JG,
    JGE,
    JB,
    JBE,
    JL,
    JLE,
    JE,
    JNE,
    JO,
    JMP,
    RET,
};

enum class InstrT {
    UNDEF,
    U8,
    U16,
    U32,
    U64,
    I8,
    I16,
    I32,
    I64,
    PHY,
    // result type
};

enum class CondCodeT { EQ, NE, LT, GE };
enum class FlowT { NONE, DEF, USE };

// related to def only
// contain instr id and bb id
// metainstruction w/ Inputs

class InstrBase {
    using UsersT = std::list<const InstrBase *>;
    using CInputListIt = typename UsersT::const_iterator;
    using InputListIt = typename UsersT::iterator;

  public:
    virtual ~InstrBase();
    InstrBase(){};
    InstrBase(const BasicBlock &bb);
    // InstrBase(const InstrBase &other);
    // InstrBase(const InstrBase &&other);
    void set_prev(const InstrBase *instr);
    void set_next(const InstrBase *instr);
    void set_bb(const BasicBlock *bb);
    void push_input(const InstrBase *instr);
    void push_inputs(initList list);
    InputListIt begin();
    InputListIt end();
    InputListIt last();
    CInputListIt erase_input(CInputListIt cit);

    virtual std::string dump() const;
    void throwIfNonConsistence_() const;
    virtual InstrT get_type() const;
    const InstrBase *get_prev() const;
    const InstrBase *get_next() const;
    const BasicBlock &get_bb() const;
    CInputListIt cbegin() const;
    CInputListIt cend() const;
    CInputListIt clast() const;
    
    id_t get_id() const;

  protected:
    const id_t m_id{ID_UNDEF};
    const BasicBlock *m_bb{nullptr};
    const InstrBase *m_prev{nullptr};
    const InstrBase *m_next{nullptr};
    UsersT m_users{};
};

class Instr : public InstrBase {
  public:
    Instr();
    Instr(const BasicBlock &bb, OpcdT opcode, InstrT type);
    Instr(const BasicBlock &bb, OpcdT opcode, InstrT type, initList list);
    std::string dump() const override;

  public:
    const OpcdT m_opcd{OpcdT::NONE};
    const InstrT m_type{InstrT::UNDEF};
};

class Phy final : public InstrBase {
  public:
    Phy();
    Phy(initList list);
    std::string dump() const override;
};

// Specific instrs classes
// class UnaryRegInstr : public InstrBase, public InputList {
//   public:
//     UnaryRegInstr(OpcdT opcd, InstrT type) : InstrBase(opcd, type) {}
// };

// class BinaryRegInstr : public InstrBase, public InputList {
//   public:
//     BinaryRegInstr(OpcdT opcd, InstrT type) : InstrBase(opcd, type) {}
// };

// class BinaryImmInstr : public InstrBase, public InputList, public ImmedOpnd<uint64_t> {
//   public:
//     BinaryImmInstr(OpcdT opcd, InstrT type) : InstrBase(opcd, type) {}
// };

// class CompareInstr : public InstrBase, public InputList, public CondOpnd {
//   public:
//     CompareInstr(OpcdT opcd, InstrT type) : InstrBase(opcd, type) {}
// };

// class CastInstr : public InstrBase, public InputList, public TypeOpnd {
//   public:
//     CastInstr(OpcdT opcd, InstrT type) : InstrBase(opcd, type) {}
// };

// class JumpInstr : public InstrBase, public ImmOpnd<uint64_t> {
//   public:
//     JumpInstr(OpcdT opcd, InstrT type) : InstrBase(opcd, type) {}
// };

// class RetInstr : public InstrBase {
//   public:
//     RetInstr(OpcdT opcd, InstrT type) : InstrBase(opcd, type) {}
// };

} // namespace IR
