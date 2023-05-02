#include "bbGraph.hpp"

namespace IR {
// G::key_t BbGraph::add_node(BasicBlock&& data) {
//     auto key = Graph::add_node(std::move(data), data.get_id());
//     if (key == ID_UNDEF) {
//         return ID_UNDEF;
//     }
//     at(key)->data().set_id(key);
//     return key;
// }

G::key_t BbGraph::add_node(BasicBlock& data, id_t id) {
    id = (id == G::KEY_UNDEF)? data.get_id() : id;
    auto key = Graph::add_node(data, id);
    if (key == ID_UNDEF) {
        return ID_UNDEF;
    }
    at(key)->data().set_id(key);
    return key;
}

void BbGraph::throwIfNotConsistent() {
    if (m_nodes.size() == 0) {
        return;
    }
    if (m_headerBbKey == G::KEY_UNDEF) {
        throw std::logic_error("Header was not set");
    }
    const IR::BasicBlock &bb = m_nodes[m_headerBbKey]->data();
    for (auto it = bb.instr_cbegin(); it != bb.instr_cend(); ++it) {
        const auto *i = *it;
        if (i->m_type != IR::GroupType::CONST) {
            throw std::logic_error("Header contain non-const data");
        }
    }
    for (const auto &node: m_nodes) {
        const auto &bb = node.second->data();
        bb.throwIfNotConsistent_();
    }
}

std::string BbGraph::dump() const {
    std::stringstream ss{};
    auto cfg = Graph::dump();
    ss << cfg;
    for (const auto &block: m_nodes) {
        ss << block.second->data().dump();
    }
    return ss.str();
}
} // namespace IR
