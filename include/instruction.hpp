#pragma once
#include "types.hpp"
#include <cstdint>
#include <exception>
#include <list>
#include <optional>
#include <string>
#include <unistd.h>
namespace IR {

class BasicBlock;
class InstrBase;

using initList = std::initializer_list<const InstrBase *>;

using ValueType = int64_t;
using RegType = uint32_t;

enum class InstrT {
    UNDEF,
    PHY,
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

enum class GroupT {
    UNDEF,
    U64,
    I64,
    DOUBLE,
    PHY,
    // result type
};

constexpr int64_t NO_VALUEHOLDER{-1};

class ValueHolder {
  public:
    // for constants
    ValueHolder() = default;
    ValueHolder(ValueType value) : m_val(value) {}
    // for reg values
    ValueHolder(ValueType value, RegType regNum) : m_val(value), m_regNum(regNum) {}
    bool isConst() const { return (m_regNum == NO_VALUEHOLDER); }
    uint32_t getRegNum() {
        if (isConst()) {
            throw std::logic_error("Constant value does not allow placeholder");
        }
        return m_regNum;
    }
    int64_t get() const {
      return m_val;
    }
    void set(int64_t val) {
      m_val = val;
    }
    void setRegNum(RegType regNum) {
      m_regNum = regNum;
    }
    void makeAsConst() {
      m_regNum = NO_VALUEHOLDER;
    }

  private:
    int64_t m_val{0};
    int64_t m_regNum{NO_VALUEHOLDER};
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
    InstrBase(const BasicBlock &bb, id_t id);
    InstrBase(id_t id);
    InstrBase(const BasicBlock &bb, id_t id, initList list);
    // InstrBase(const InstrBase &other);
    // InstrBase(const InstrBase &&other);
    void set_prev(const InstrBase *instr);
    void set_next(const InstrBase *instr);
    void set_bb(const BasicBlock *bb);
    void set_id(id_t id);
    void push_input(const InstrBase *instr);
    void push_inputs(initList list);
    void forget_dependencies();
    InputListIt begin();
    InputListIt end();
    InputListIt last();
    InputListIt erase_input(InputListIt cit);
    bool erase_input(id_t bb_id, id_t id);

    virtual std::string dump() const;
    void throwIfNotConsistent_() const;
    virtual GroupT get_type() const;
    const InstrBase *get_prev() const;
    const InstrBase *get_next() const;
    const BasicBlock &get_bb() const;
    CInputListIt cbegin() const;
    CInputListIt cend() const;
    CInputListIt clast() const;

    id_t get_id() const;

  protected:
    id_t m_id{ID_UNDEF};
    const BasicBlock *m_bb{nullptr};
    const InstrBase *m_prev{nullptr};
    const InstrBase *m_next{nullptr};
    UsersT m_users{};
};

class Instr : public InstrBase {
  public:
    Instr();
    Instr(id_t id);
    Instr(const BasicBlock &bb, id_t id, InstrT opcode, GroupT type, int64_t value, int64_t valueHolder);
    std::string dump() const override;

  public:
    const InstrT m_opcd{InstrT::UNDEF};
    const GroupT m_type{GroupT::UNDEF};
    ValueHolder m_value{};
};

class Phy final : public InstrBase {
  public:
    Phy();
    Phy(id_t id);
    Phy(const BasicBlock &bb, id_t id, initList list);
    std::string dump() const override;
};

// Specific instrs classes
// class UnaryRegInstr : public InstrBase, public InputList {
//   public:
//     UnaryRegInstr(InstrT opcd, GroupT type) : InstrBase(opcd, type) {}
// };

// class BinaryRegInstr : public InstrBase, public InputList {
//   public:
//     BinaryRegInstr(InstrT opcd, GroupT type) : InstrBase(opcd, type) {}
// };

// class BinaryImmInstr : public InstrBase, public InputList, public ImmedOpnd<uint64_t> {
//   public:
//     BinaryImmInstr(InstrT opcd, GroupT type) : InstrBase(opcd, type) {}
// };

// class CompareInstr : public InstrBase, public InputList, public CondOpnd {
//   public:
//     CompareInstr(InstrT opcd, GroupT type) : InstrBase(opcd, type) {}
// };

// class CastInstr : public InstrBase, public InputList, public TypeOpnd {
//   public:
//     CastInstr(InstrT opcd, GroupT type) : InstrBase(opcd, type) {}
// };

// class JumpInstr : public InstrBase, public ImmOpnd<uint64_t> {
//   public:
//     JumpInstr(InstrT opcd, GroupT type) : InstrBase(opcd, type) {}
// };

// class RetInstr : public InstrBase {
//   public:
//     RetInstr(InstrT opcd, GroupT type) : InstrBase(opcd, type) {}
// };

} // namespace IR
