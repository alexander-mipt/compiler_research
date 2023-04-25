#pragma once
#include "graph.hpp"
#include "basicblock.hpp"

namespace IR {
using G::Graph;

struct Metadata {

};

class BbGraph : public Graph<BasicBlock, int> {
    public:
    // G::key_t add_node(BasicBlock&& node_data) override;
    /// @brief add node with given key; if no key provided - use bb key; if bb key is undefined - use graph keygen and set bb_id = key
    /// @return node key or KEY_UNDEF in case of error
    G::key_t add_node(BasicBlock& node_data, id_t key = G::KEY_UNDEF) override;
    std::string dump() const override;
    /// @brief get header basic block; it should contain constant & param nodes
    /// @return node key or KEY_UNDEF if not found
    key_t setHeader(key_t key) {
        m_headerBbKey = key;
        return key;
    }
    G::Node<BasicBlock, int> *accessHeader() {
        if (m_nodes.find(m_headerBbKey) == m_nodes.end()) {
            return nullptr;
        }
        return m_nodes[m_headerBbKey];
    }
    private:
    key_t m_headerBbKey{G::KEY_UNDEF};
    Metadata m_metadata; 
};
} // namespace IR