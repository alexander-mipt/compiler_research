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

using initList = std::initializer_list<InstrBase *>;

using ValueType = int64_t;
using RegType = int64_t;

enum class OpcodeType {
    UNDEF,
    // FCONST,
    ICONST,
    MOVI,
    PHY,
    // CAST,
    // ADDI,
    ADD,
    // SUBI,
    // SUB,
    // MULI,
    // MUL,
    // MOVI,
    AND,
    XOR,
    SHL,
    // MOV,
    // CMP,
    // JA,
    // JAE,
    // JG,
    // JGE,
    // JB,
    // JBE,
    // JL,
    // JLE,
    // JE,
    // JNE,
    // JO,
    // JMP,
    // RET,
};

enum class GroupType {
    UNDEF,
    GENERAL64,
    CONST,
    // DOUBLE,
    PHY,
    CHECK,
    // result type
};

constexpr ValueType NO_VALUEHOLDER{-1};
struct ValueHolder {
    ValueHolder() = default;
    ValueHolder(ValueType val, RegType reg) : m_val(val), m_regNum(reg) {
      if (reg < NO_VALUEHOLDER) {
        throw std::logic_error("Wrong reg");
      }
    }
    ValueType m_val{0};
    RegType m_regNum{NO_VALUEHOLDER};
};

enum class CondCodeT { EQ, NE, LT, GE };
enum class FlowT { NONE, DEF, USE };

// related to def only
// contain instr id and bb id
// metainstruction w/ Inputs

class InstrBase {
  public:
    using UsersT = std::list<InstrBase *>;
    using InputsT = std::list<InstrBase *>;
    using InputListIt = typename InputsT::iterator;
    using UserListIt = typename UsersT::iterator;

  public:
    virtual ~InstrBase();
    InstrBase(){};
    InstrBase(id_t id);
    // InstrBase(BasicBlock &bb);
    InstrBase(BasicBlock &bb, id_t id);
    // InstrBase(id_t id);
    // InstrBase(const BasicBlock &bb, id_t id, initList list);

    void set_prev(InstrBase *instr);
    void set_next(InstrBase *instr);
    void set_bb(BasicBlock *bb);
    void set_id(id_t id);

    void push_input(InstrBase *instr);
    void push_inputs(initList list);
    void push_user(InstrBase *instr);
    void push_users(initList list);

    void forget_dependencies();
    InputListIt inputs_begin();
    InputListIt inputs_end();
    InputListIt inputs_last();
    bool onlyOneInput() const;
    bool onlyOneUser() const;
    // InputListIt erase_input(InputListIt cit);
    bool erase_input(id_t bb_id, id_t id);
    UserListIt users_begin();
    UserListIt users_end();
    UserListIt users_last();
    // UserListIt erase_user(UserListIt cit);
    bool erase_user(id_t bb_id, id_t id);
    UsersT &users() &;
    InputsT &inputs() &;

    virtual std::string dump() const;
    void throwIfNotConsistent_() const;
    void throwIfWrongDeps_() const;
    virtual GroupType type() const = 0;
    InstrBase *prev() const;
    InstrBase *next() const;
    BasicBlock *bb() const;
    id_t get_id() const;

  protected:
    id_t m_id{ID_UNDEF};
    BasicBlock *m_bb{nullptr};
    InstrBase *m_prev{nullptr};
    InstrBase *m_next{nullptr};
    UsersT m_users{};
    InputsT m_inputs{};
};

class Instr : public InstrBase {
  public:
    Instr();
    // Instr(id_t id);
    Instr(id_t id, OpcodeType opcode, GroupType type, ValueHolder value);
    std::string dump() const override;
    GroupType type() const override { return m_type; }
    void throwIfWrongInputCount_() const;

  public:
    OpcodeType m_opcd{OpcodeType::UNDEF};
    GroupType m_type{GroupType::UNDEF};
    ValueHolder m_value{};
};

class Phy final : public InstrBase {
  public:
    Phy();
    Phy(id_t id);
    std::string dump() const override;
    GroupType type() const override { return GroupType::PHY; }
};

} // namespace IR
