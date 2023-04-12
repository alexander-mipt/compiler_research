#pragma once
#include "graph.hpp"
#include "basicblock.hpp"

namespace IR {
using G::Graph;

struct Metadata {

};

class BbGraph : public Graph<BasicBlock, int> {
    public:
    G::key_t add_node(BasicBlock&& node_data) override;
    G::key_t add_node(BasicBlock&& node_data, id_t key) override;
    private:
    Metadata m_metadata; 
};
} // namespace IR