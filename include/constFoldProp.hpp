#include "bbGraph.hpp"
#include "instruction.hpp"

void processInstr(IR::Instr &i) {
    
}

bool doConstFoldProp(IR::BbGraph &g) {
    if (g.get_node_count() < 1) {
        return false;
    }
    auto lastBbIt = --g.nodes_end();
    auto &lastInstr = lastBbIt->second->access_data();
}