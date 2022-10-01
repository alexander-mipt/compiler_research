#include "instruction.hpp"
#include <utility>
#include <list>
#include <array>

namespace IR {

using phi_t = std::pair<Instr*, Instr*>;

class BasicBlock {
    public:
    private:
    Instr *m_first{nullptr};
    Instr *m_second{nullptr};
    std::list<BasicBlock*> m_predececors{};
    std::array<BasicBlock*, 2> m_successors{};
    graph_t *m_graph{nullptr};
};
}