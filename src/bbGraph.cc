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
