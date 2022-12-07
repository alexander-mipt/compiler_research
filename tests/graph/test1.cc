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