#pragma once
#include "types.hpp"
#include <cstdint>
#include <list>
#include <optional>
#include <string>
#include <unistd.h>

namespace IR {

class BasicBlock;

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
    NONE,
    U8,
    U16,
    U32,
    U64,
    I8,
    I16,
    I32,
    I64,
    // result type
};

enum class CondCodeT { EQ, NE, LT, GE };

enum class FlowT { NONE, DEF, USE };

class InstrBase;

// related to def only
// contain instr id and bb id
struct InstrInput {
    InstrInput(const InstrBase *instr, FlowT flowType) : m_instr(instr), m_flow(flowType) {}
    virtual ~InstrInput(){};

    virtual std::string dump() const;
    virtual bool is_phi() const { return false; }

    const InstrBase *const m_instr{nullptr};
    const FlowT m_flow{FlowT::NONE};
};

// metainstruction w/ Inputs
struct PhiInstr final : public InstrInput {
    PhiInstr(const InstrBase *instr, FlowT flowType) : InstrInput(instr, flowType){};
    PhiInstr(const InstrBase *instr, FlowT flowType, const std::list<InstrInput> &&list)
        : InstrInput(instr, flowType), m_others(std::move(list)){};
    std::string dump() const override;
    bool is_phi() const override { return true; }
    
    std::list<InstrInput> m_others{};
};
class InstrBase {
    using CInputListIt = typename std::list<InstrInput *>::const_iterator;

  public:
    virtual ~InstrBase();
    InstrBase(const BasicBlock &bb, OpcdT opcode, InstrT type);
    InstrBase(const InstrBase &other);
    InstrBase(const InstrBase &&other);
    void set_prev(InstrBase *instr) { m_prev = instr; }
    void set_next(InstrBase *instr) { m_next = instr; }
    void set_bb(const BasicBlock *bb);
    void push_input(const InstrInput &&elem);
    void erase_input(CInputListIt cit);

    virtual std::string dump() const;
    const InstrBase *get_prev() const { return m_prev; }
    const InstrBase *get_next() const { return m_next; }
    const BasicBlock &get_bb() const;
    CInputListIt cbegin() const { return m_users.cbegin(); }
    CInputListIt cend() const { return m_users.cend(); }
    CInputListIt clast() const { return --m_users.cend(); }
    id_t get_id() const { return m_id; }

  public:
    const OpcdT m_opcd{OpcdT::NONE};
    const InstrT m_type{InstrT::NONE};

  protected:
    static id_t g_id;
    const id_t m_id{ID_UNDEF};
    const BasicBlock *m_bb{nullptr};
    InstrBase *m_prev{nullptr};
    InstrBase *m_next{nullptr};
    std::list<InstrInput *> m_users{};
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
