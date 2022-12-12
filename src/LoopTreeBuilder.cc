#include "LoopTreeBuilder.hpp"

namespace Analysis {
template <> void buildLoopTree(G::Graph<int, int> *g, G::Graph<G::Cycle, int> *loops) {
    if (g == nullptr || loops == nullptr) {
        return;
    }
    // create loop nodes
    auto v = g->get_cycles(1, -1);
    for (auto &cycle : v) {
        OPT(std::cout << cycle.dump() << std::endl;);
        loops->add_node(std::move(cycle), cycle.back_edge_start);
    }

    // generate RPO round
    auto rpo = g->RPO(1);
#ifndef NDEBUG_DEV
    std::cout << "RPO: ";
    for (auto elem : rpo) {
        std::cout << elem << "\t";
    }
    std::cout << "\n";
    auto dfs = g->DFS(1);
    std::cout << "DFS: ";
    for (auto elem : dfs) {
        std::cout << elem << "\t";
    }
    std::cout << "\n";
#endif

    // populate loops
    for (auto it = rpo.rbegin(), rend = rpo.rend(); it != rend; ++it) {
        if (!loops->node_exists(*it) /*|| !loops.at(*it)->access_data().reducible*/) {
            continue;
        }
        auto current_loop = *it;
        OPT(LOGarg("Work with loop", current_loop));
        // link inner loops
        for (auto key : g->DFS(loops->at(current_loop)->access_data().back_edge_start)) {
            OPT(LOGarg("\tfound block:", key));
            if (g->at(key)->get_loop() == G::KEY_UNDEF) {
                g->at(key)->set_loop(current_loop);
            } else {
                auto outer_loop = g->at(key)->get_loop();
                g->at(key)->set_loop(current_loop);
                OPT(std::cout << "Adding edge" << outer_loop << " -> " << current_loop << std::endl;);
                loops->add_edge(0, outer_loop, current_loop);
            }
        }
    }

    for (auto it = g->nodes_begin(); it != g->nodes_end(); ++it) {
        if (it->second->get_loop() == G::KEY_UNDEF) {
            key_t root_loop_key = g->get_node_count() + 1;
            loops->add_node(G::Cycle{}, root_loop_key);
            loops->at(root_loop_key)->access_data().blocks.push_back(it->second->get_key());
        } else {
            ASSERT_DEV(loops->at(it->second->get_loop()) != nullptr, "Wrong loop node");
            loops->at(it->second->get_loop())->access_data().blocks.push_back(it->second->get_key());
        }
    }
}
} // namespace Analysis