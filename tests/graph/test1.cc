#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "graph.hpp"

TEST_CASE("Test API 1", "[graph1]") {
    G::Graph<int, int> g{};
    G::key_t edge_key{1};
    int data = 5;
    
    for (key_t i = 1; i <= 5; ++i) {
        REQUIRE(g.add_node(data, i) != G::KEY_UNDEF);
    }
    REQUIRE(g.add_edge(data, 1, 3, edge_key++) != G::KEY_UNDEF); // 1
    REQUIRE(g.add_edge(data, 3, 2, edge_key++) != G::KEY_UNDEF); // 2
    REQUIRE(g.add_edge(data, 3, 1, edge_key++) != G::KEY_UNDEF); // 3
    REQUIRE(g.add_edge(data, 2, 4, edge_key++) != G::KEY_UNDEF); // 4
    REQUIRE(g.add_edge(data, 3, 4, edge_key++) != G::KEY_UNDEF); // 5
    REQUIRE(g.add_edge(data, 2, 5, edge_key++) != G::KEY_UNDEF); // 6
    REQUIRE(g.add_edge(data, 1, 5, edge_key++) != G::KEY_UNDEF); // 7
    REQUIRE(g.add_edge(data, 3, 5, edge_key++) != G::KEY_UNDEF); // 8
    REQUIRE(g.add_edge(data, 5, 1, edge_key++) != G::KEY_UNDEF); // 9
    REQUIRE(g.delete_edge(1, 5, 7) != G::KEY_UNDEF);
    REQUIRE(g.delete_edge(5, 1, 9) != G::KEY_UNDEF);

    std::cerr << g.dump();
}

TEST_CASE("Test API 2", "[graph2]") {

    G::Graph<int, int> d{};
    G::key_t edge_key{1};
    int data{5};

    for (key_t i = 1; i <= 5; ++i) {
        REQUIRE(d.add_node(data, i) != G::KEY_UNDEF);
    }
    REQUIRE(d.add_edge(data, 1, 3, edge_key++) != G::KEY_UNDEF); // 1
    REQUIRE(d.add_edge(data, 3, 2, edge_key++) != G::KEY_UNDEF); // 2
    REQUIRE(d.add_edge(data, 3, 1, edge_key++) != G::KEY_UNDEF); // 3
    REQUIRE(d.add_edge(data, 2, 4, edge_key++) != G::KEY_UNDEF); // 4
    REQUIRE(d.add_edge(data, 3, 4, edge_key++) != G::KEY_UNDEF); // 5
    REQUIRE(d.add_edge(data, 2, 5, edge_key++) != G::KEY_UNDEF); // 6
    REQUIRE(d.add_edge(data, 1, 5, edge_key++) != G::KEY_UNDEF); // 7
    REQUIRE(d.add_edge(data, 3, 5, edge_key++) != G::KEY_UNDEF); // 8
    REQUIRE(d.add_edge(data, 5, 1, edge_key++) != G::KEY_UNDEF); // 9
    REQUIRE(d.delete_edge(1, 5, 7) != G::KEY_UNDEF);
    REQUIRE(d.delete_edge(5, 1, 9) != G::KEY_UNDEF);
    REQUIRE(d.delete_node(3) != G::KEY_UNDEF);
    std::cerr << d.dump();
}