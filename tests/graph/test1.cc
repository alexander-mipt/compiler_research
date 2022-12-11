#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "graph.hpp"

TEST_CASE("Test API 1", "[graph1]") {
    G::Graph<int, int> g{};

    for (key_t i = 1; i <= 5; ++i) {
        REQUIRE(g.add_node(10) != G::KEY_UNDEF);
    }
    REQUIRE(g.add_edge(10, 1, 3) != G::KEY_UNDEF); // 1
    REQUIRE(g.add_edge(10, 3, 2) != G::KEY_UNDEF); // 2
    REQUIRE(g.add_edge(10, 3, 1) != G::KEY_UNDEF); // 3
    REQUIRE(g.add_edge(10, 2, 4) != G::KEY_UNDEF); // 4
    REQUIRE(g.add_edge(10, 3, 4) != G::KEY_UNDEF); // 5
    REQUIRE(g.add_edge(10, 2, 5) != G::KEY_UNDEF); // 6
    REQUIRE(g.add_edge(10, 1, 5) != G::KEY_UNDEF); // 7
    REQUIRE(g.add_edge(10, 3, 5) != G::KEY_UNDEF); // 8
    REQUIRE(g.add_edge(10, 5, 1) != G::KEY_UNDEF); // 9
    REQUIRE(g.delete_edge(1, 5) != G::KEY_UNDEF);
    REQUIRE(g.delete_edge(5, 1) != G::KEY_UNDEF);

    std::cerr << g.dump();
}

TEST_CASE("Test API 2", "[graph2]") {

    G::Graph<int, int> d{};
    G::key_t edge_key{1};

    for (key_t i = 1; i <= 5; ++i) {
        REQUIRE(d.add_node(10) != G::KEY_UNDEF);
    }
    REQUIRE(d.add_edge(10, 1, 3) != G::KEY_UNDEF); // 1
    REQUIRE(d.add_edge(10, 3, 2) != G::KEY_UNDEF); // 2
    REQUIRE(d.add_edge(10, 3, 1) != G::KEY_UNDEF); // 3
    REQUIRE(d.add_edge(10, 2, 4) != G::KEY_UNDEF); // 4
    REQUIRE(d.add_edge(10, 3, 4) != G::KEY_UNDEF); // 5
    REQUIRE(d.add_edge(10, 2, 5) != G::KEY_UNDEF); // 6
    REQUIRE(d.add_edge(10, 1, 5) != G::KEY_UNDEF); // 7
    REQUIRE(d.add_edge(10, 3, 5) != G::KEY_UNDEF); // 8
    REQUIRE(d.add_edge(10, 5, 1) != G::KEY_UNDEF); // 9
    REQUIRE(d.delete_edge(1, 5) != G::KEY_UNDEF);
    REQUIRE(d.delete_edge(5, 1) != G::KEY_UNDEF);
    REQUIRE(d.delete_node(3) != G::KEY_UNDEF);
    std::cerr << d.dump();
}

TEST_CASE("Test API 3", "[graph3]") {

    G::Graph<int, int> d{};
    G::key_t edge_key{1};

    for (key_t i = 1; i <= 5; ++i) {
        REQUIRE(d.add_node(10) != G::KEY_UNDEF);
    }
    REQUIRE(d.add_edge(10, 1, 3) != G::KEY_UNDEF);
    REQUIRE(d.add_edge(10, 1, 3) == G::KEY_DUBLICATE);
    REQUIRE(d.delete_edge(1, 3) != G::KEY_UNDEF);
    REQUIRE(d.delete_edge(1, 3) == G::KEY_UNDEF);

    REQUIRE(d.add_edge(10, 2, 5) != G::KEY_UNDEF);
    REQUIRE(d.add_edge(10, 2, 4) != G::KEY_UNDEF);
    REQUIRE(d.delete_node(2) != G::KEY_UNDEF);
    REQUIRE(d.delete_edge(2, 5) == G::KEY_UNDEF);
    REQUIRE(d.delete_edge(2, 4) == G::KEY_UNDEF);
    REQUIRE(d.add_edge(10, 2, 4) == G::KEY_UNDEF);
    REQUIRE(d.add_edge(10, 2, 2) == G::KEY_UNDEF);

    std::cerr << d.dump();
}

TEST_CASE("Test API 4", "[graph4]") {

    G::Graph<int, int> d{};
    G::key_t edge_key{1};

    REQUIRE(d.add_edge(10, 1, 3) == G::KEY_UNDEF);
    REQUIRE(d.delete_edge(1, 3) == G::KEY_UNDEF);
    REQUIRE(d.delete_node(1) == G::KEY_UNDEF);

    std::cerr << d.dump();
}

/*
    DFS & RPO test #1
          ┌───┐
          │ 1 │
          └───┘
            │
            │
            ▼
┌───┐     ┌───┐
│ 3 │ ◀── │ 2 │
└───┘     └───┘
  │         │
  │         │
  │         ▼
  │       ┌───┐     ┌───┐
  │       │ 6 │ ──▶ │ 7 │
  │       └───┘     └───┘
  │         │         │
  │         │         │
  │         ▼         │
  │       ┌───┐       │
  │       │ 5 │       │
  │       └───┘       │
  │         │         │
  │         │         │
  │         ▼         │
  │       ┌───┐       │
  └─────▶ │ 4 │ ◀─────┘
          └───┘

*/

TEST_CASE("Test DFS", "[DFS1]") {

    G::Graph<int, int> g{};

    for (key_t i = 1; i <= 7; ++i) {
        REQUIRE(g.add_node(10) != G::KEY_UNDEF);
    }
    REQUIRE(g.add_edge(10, 1, 2) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(10, 2, 3) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(10, 3, 4) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(10, 5, 4) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(10, 6, 5) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(10, 6, 7) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(10, 7, 4) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(10, 2, 6) != G::KEY_UNDEF);
    auto vector = g.DFS(1, 4);
    auto vector2 = g.RPO(1);

    std::cerr << g.dump();
    for (auto elem : vector) {
        std::cerr << elem << "\t";
    }
    std::cerr << "\n";
    for (auto elem : vector2) {
        std::cerr << elem << "\t";
    }
    std::cerr << "\n";
}

TEST_CASE("Test dom search", "[DOM1]") {
    G::Graph<int, int> g{};

    for (key_t i = 1; i <= 7; ++i) {
        REQUIRE(g.add_node(10) != G::KEY_UNDEF);
    }
    REQUIRE(g.add_edge(10, 1, 2) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(10, 2, 3) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(10, 3, 4) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(10, 5, 4) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(10, 6, 5) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(10, 6, 7) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(10, 7, 4) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(10, 2, 6) != G::KEY_UNDEF);

    std::cerr << g.dump();

    // auto vector = g.getDominatedNodes(1, 1);
    // for (auto elem : vector) {
    //     std::cerr << elem << "\t";
    // }
    // std::cerr << "\n";

    auto vector = g.getDominatedNodes(1, 6);
    for (auto elem : vector) {
        std::cerr << elem << "\t";
    }
    std::cerr << "\n";

    // vector = g.getDominatedNodes(1, 2);
    // for (auto elem : vector) {
    //     std::cerr << elem << "\t";
    // }
    // std::cerr << "\n";

    vector = g.getDominatedNodes(1, 7);
    for (auto elem : vector) {
        std::cerr << elem << "\t";
    }
    std::cerr << "\n";
}

TEST_CASE("Test dom tree", "[DOM_TREE_1]") {
    // create SSA graph
    G::Graph<int, int> g{};
    REQUIRE(g.add_nodes(0, 7) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(0, 1, 2) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(0, 2, 3) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(0, 3, 4) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(0, 5, 4) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(0, 6, 5) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(0, 6, 7) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(0, 7, 4) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(0, 2, 6) != G::KEY_UNDEF);
    REQUIRE(g.hasPath(1, 4));
    REQUIRE(g.hasPath(6, 4));
    REQUIRE(g.hasPath(2, 4));
    REQUIRE(g.hasPath(5, 4));

    G::Graph<int, int> domTree{};
    REQUIRE(domTree.add_nodes(0, g.get_node_count()) != G::KEY_UNDEF);
    auto sortedNodes = g.DFS(1);
    for (auto it = sortedNodes.rbegin(); it != sortedNodes.rend(); ++it) {
        auto &key = *it;
        std::cout << "Getting employees for " << key << std::endl;
        auto dominated_node_keys = g.getDominatedNodes(1, key);
        for (auto elem : dominated_node_keys) {
            std::cerr << elem << "\t";
        }
        std::cerr << "\n";
        for (auto it = ++dominated_node_keys.begin(); it != dominated_node_keys.end(); ++it) {
            key_t start = dominated_node_keys.front();
            key_t end = *it;
            std::cout << "\tneed to add path from " << start << " to " << end << std::endl;
            if (domTree.at(end) != nullptr && domTree.at(end)->get_predecessor_count() == 0) {
                domTree.add_edge(0, start, end);
            }
        }
    }

    std::cerr << g.dump();
    std::cerr << domTree.dump();
}

/*
          ┌────┐
          │ 1  │
          └────┘
            │
            │
            ▼
          ┌────┐
  ┌─────▶ │ 2  │ ─┐
  │       └────┘  │
  │         │     │
  │         │     │
  │         ▼     │
  │       ┌────┐  │
  │       │ 10 │  │
  │       └────┘  │
  │         │     │
  │         │     │
  │         ▼     │
  │       ┌────┐  │
  │       │ 3  │ ◀┘
  │       └────┘
  │         │
  │         │
  │         ▼
  │       ┌────┐
  │       │ 4  │
  │       └────┘
  │         │
  │         │
  │         ▼
  │       ┌────┐
  │       │ 5  │
  │       └────┘
  │         │
  │         │
  │         ▼
  │       ┌────┐
  │       │ 6  │
  │       └────┘
  │         │
  │         │
  │         ▼
┌───┐     ┌────┐
│ 8 │ ◀── │ 7  │
└───┘     └────┘
            │
            │
            ▼
          ┌────┐
          │ 9  │
          └────┘
            │
            │
            ▼
          ┌────┐
          │ 11 │
          └────┘
*/
TEST_CASE("Test dom tree 2", "[DOM_TREE_2]") {
    // create SSA graph
    G::Graph<int, int> g{};
    REQUIRE(g.add_nodes(0, 11) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(0, 1, 2) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(0, 2, 3) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(0, 3, 4) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(0, 4, 5) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(0, 5, 6) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(0, 6, 7) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(0, 7, 8) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(0, 8, 2) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(0, 7, 9) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(0, 9, 11) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(0, 2, 10) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(0, 10, 3) != G::KEY_UNDEF);

    G::Graph<int, int> domTree{};
    REQUIRE(domTree.add_nodes(0, g.get_node_count()) != G::KEY_UNDEF);
    auto sortedNodes = g.DFS(1);
    std::cout << "DFS:\t";
    for (auto elem : sortedNodes) {
        std::cout << elem << "\t";
    }
    std::cout << std::endl;
    for (auto it = sortedNodes.rbegin(); it != sortedNodes.rend(); ++it) {
        auto &key = *it;
        std::cout << "Getting employees for " << key << std::endl;
        auto dominated_node_keys = g.getDominatedNodes(1, key);
        for (auto elem : dominated_node_keys) {
            std::cerr << elem << "\t";
        }
        std::cerr << "\n";
        for (auto it = ++dominated_node_keys.begin(); it != dominated_node_keys.end(); ++it) {
            key_t start = dominated_node_keys.front();
            key_t end = *it;
            std::cout << "\tneed to add path from " << start << " to " << end << std::endl;
            if (domTree.at(end) != nullptr && domTree.at(end)->get_predecessor_count() == 0) {
                domTree.add_edge(0, start, end);
            }
        }
    }

    std::cerr << g.dump();
    std::cerr << domTree.dump();
}

/*
       ┌────────────────────────┐
       │                        │
       │                 ┌───┐  │
       │                 │ 1 │  │
       │                 └───┘  │
       │                   │    │
       │                   │    │
       │                   ▼    │
     ┌───┐     ┌───┐     ┌───┐  │
     │ 6 │ ◀── │ 5 │ ◀── │ 2 │ ◀┘
     └───┘     └───┘     └───┘
       │         │         │
       │         │         │
       ▼         │         ▼
     ┌───┐       │       ┌───┐
  ┌─ │ 8 │       │       │ 3 │ ◀┐
  │  └───┘       │       └───┘  │
  │    │         │         │    │
  │    │         │         │    │
  │    │         │         ▼    │
  │    │         │       ┌───┐  │
  │    │         └─────▶ │ 4 │  │
  │    │                 └───┘  │
  │    │                   │    │
  │    │                   │    │
  │    │                   ▼    │
  │    │                 ┌───┐  │
  │    └───────────────▶ │ 7 │ ─┘
  │                      └───┘
  │                        │
  │                        │
  │                        ▼
  │                      ┌───┐
  │                      │ 9 │
  │                      └───┘
  │                        ▲
  └────────────────────────┘
*/
TEST_CASE("Test dom tree 3", "[DOM_TREE_3]") {
    // create SSA graph
    G::Graph<int, int> g{};
    REQUIRE(g.add_nodes(0, 9) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(0, 1, 2) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(0, 2, 3) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(0, 3, 4) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(0, 4, 7) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(0, 7, 9) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(0, 7, 3) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(0, 2, 5) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(0, 5, 6) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(0, 6, 8) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(0, 8, 7) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(0, 8, 9) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(0, 6, 2) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(0, 5, 4) != G::KEY_UNDEF);

    G::Graph<int, int> domTree{};
    REQUIRE(domTree.add_nodes(0, g.get_node_count()) != G::KEY_UNDEF);
    auto sortedNodes = g.DFS(1);
    std::cout << "DFS:\t";
    for (auto elem : sortedNodes) {
        std::cout << elem << "\t";
    }
    std::cout << std::endl;
    for (auto it = sortedNodes.rbegin(); it != sortedNodes.rend(); ++it) {
        auto &key = *it;
        std::cout << "Getting employees for " << key << std::endl;
        auto dominated_node_keys = g.getDominatedNodes(1, key);
        for (auto elem : dominated_node_keys) {
            std::cerr << elem << "\t";
        }
        std::cerr << "\n";
        for (auto it = ++dominated_node_keys.begin(); it != dominated_node_keys.end(); ++it) {
            key_t start = dominated_node_keys.front();
            key_t end = *it;
            std::cout << "\tneed to add path from " << start << " to " << end << std::endl;
            if (domTree.at(end) != nullptr && domTree.at(end)->get_predecessor_count() == 0) {
                domTree.add_edge(0, start, end);
            }
        }
    }

    std::cerr << g.dump();
    std::cerr << domTree.dump();
}

TEST_CASE("Test graph buffer", "[Gbuf1]") {

    G::Graph<int, int> g{};

    for (key_t i = 1; i <= 7; ++i) {
        REQUIRE(g.add_node(10) != G::KEY_UNDEF);
    }
    REQUIRE(g.add_edge(10, 1, 2) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(10, 2, 3) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(10, 3, 4) != G::KEY_UNDEF);
    REQUIRE(g.cut_node(2) != G::KEY_UNDEF);
    REQUIRE(g.cut_edge(3, 4) != G::KEY_UNDEF);
    REQUIRE(g.cut_node(1) != G::KEY_UNDEF);
    REQUIRE(g.cut_node(2) == G::KEY_UNDEF);
    REQUIRE(g.delete_node(2) == G::KEY_UNDEF);
    // REQUIRE(g.cut_node(3) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(10, 4, 4) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(10, 4, 4) == G::KEY_DUBLICATE);
    REQUIRE(g.cut_edge(4, 4) != G::KEY_UNDEF);
    REQUIRE(g.add_node(10) != G::KEY_UNDEF);
    REQUIRE(g.cut_node(8) != G::KEY_UNDEF);
    REQUIRE(g.add_node(10) != G::KEY_UNDEF);
    REQUIRE(g.cut_node(9) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(10, 4, 6) != G::KEY_UNDEF);
    REQUIRE(g.add_edge(10, 6, 3) != G::KEY_UNDEF);
    REQUIRE(g.cut_node(4) != G::KEY_UNDEF);
    std::cerr << g.dump();
    REQUIRE(g.paste_all() != G::KEY_UNDEF);
    std::cerr << g.dump();
}