#include "bbGraph.hpp"

namespace IR {
G::key_t BbGraph::add_node(BasicBlock&& data) {
    auto key = Graph::add_node(std::move(data), data.get_id());
    if (key == ID_UNDEF) {
        return ID_UNDEF;
    }
    at(key)->access_data().set_id(key);
    return key;
}

G::key_t BbGraph::add_node(BasicBlock&& data, id_t id) {
    auto key = Graph::add_node(std::move(data), id);
    if (key == ID_UNDEF) {
        return ID_UNDEF;
    }
    at(key)->access_data().set_id(key);
    return key;
}
} // namespace IR
