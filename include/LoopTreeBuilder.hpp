#pragma once
#include "graph.hpp"

namespace Analysis {
template <typename NodeT> void buildLoopTree(G::Graph<NodeT, int> *g, G::Graph<G::Cycle, int> *loops) {}
}