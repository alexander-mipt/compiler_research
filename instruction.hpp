#pragma once
#include <cstdint>
#include <unistd.h>

namespace IR {

enum opcode_t {
    OPC_UNDEF,
};

enum instr_t {
    TYP_UNDEF,
    TYP_UNARY,
    TYP_BINARY,
    TYP_CAST,
    TYP_MEM,
    TYP_BRANCH,
    // result type
};

struct bb_info {
    uint64_t bb_id{0};
};

// base struct
struct Instr {
    virtual ~Instr(){};
    Instr();
    Instr(opcode_t opcode, instr_t type);
    // virtual clone();
    // Instr(const Instr&);

    const opcode_t m_opcd{OPC_UNDEF};
    const instr_t m_type{TYP_UNDEF};
    Instr *m_prev{nullptr};
    Instr *m_next{nullptr};
    int instr_id{};
    // list of inputs (uses)
    // list of outputs (defs)
    bb_info m_bb{};
};

struct UnaryOp : public Instr {
    UnaryOp(opcode_t opcode);
};

struct BinaryOp : public Instr {
    BinaryOp(opcode_t opcode);
};

struct CastOp : public Instr {
    CastOp(opcode_t opcode);
};

struct MemOp : public Instr {
    MemOp(opcode_t opcode);
};

struct BranchOp : public Instr {
    BranchOp(opcode_t opcode);
};

struct PHI {
    PHI();
};

} // namespace IR
