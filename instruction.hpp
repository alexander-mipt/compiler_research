#pragma once
#include <cstdint>
#include <unistd.h>

namespace IR {
using opcode_t = uint8_t;

enum instr_t {
    UNDEF,
};

struct bb_info {
    uint64_t bb_id{0};
};

// base class
class Instr {
  public:
    virtual ~Instr(){};

  protected:
    opcode_t m_opcd{0};
    instr_t m_type{UNDEF};
    bb_info m_bb{};
    Instr *m_prev{nullptr};
    Instr *m_next{nullptr};
};

class Branch : public Instr {

};


} // namespace IR
