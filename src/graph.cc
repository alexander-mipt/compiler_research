#include "graph.hpp"

G::Cycle::Cycle(key_t head_id, key_t back_edge_id, key_t back_edge_start_id, bool reducible)
    : head_node(head_id), back_edge(back_edge_id), back_edge_start(back_edge_start_id),
      is_reducible(reducible) {}

std::string G::Cycle::dump() const {
    std::stringstream ss{};
    ss $("Cycle: ") $int(head_node) $int(back_edge) $int(back_edge_start) $intn(is_reducible);
    return ss.str();
}
