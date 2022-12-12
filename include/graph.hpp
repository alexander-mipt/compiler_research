#pragma once
#include "config.hpp"
#include <exception>
#include <list>
#include <map>
#include <optional>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace G {
using key_t = long long;
constexpr key_t KEY_UNDEF = 0;
constexpr key_t KEY_DUBLICATE = -1;

enum class ColorT {
    WHITE,
    GRAY,
    BLACK,
    HIDDEN,
};

struct Cycle {
    key_t head_node{KEY_UNDEF};
    key_t back_edge{KEY_UNDEF};
    key_t back_edge_start{KEY_UNDEF};
    bool reducible{false};
};

using weight_t = int;

template <typename N, typename E> class Graph;

// N - type of additional data
template <typename N, typename E> class Node {

  public:
    Node(N &&data, const Graph<N, E> &g);
    virtual ~Node(){};
    key_t add_successor(key_t s_key);
    key_t add_predecessor(key_t p_key);
    key_t delete_successor(key_t s_key);
    key_t delete_predecessor(key_t p_key);

    typename std::map<key_t, Node<N, E> *>::iterator predecessors_begin();
    typename std::map<key_t, Node<N, E> *>::iterator predecessors_end();
    typename std::map<key_t, Node<N, E> *>::iterator successors_begin();
    typename std::map<key_t, Node<N, E> *>::iterator successors_end();

    void set_color(ColorT c);
    void set_loop(key_t header_key);
    N &access_data() &;
    const key_t get_key() const;
    ColorT get_color() const;
    key_t get_loop() const;
    size_t get_predecessor_count() const;
    size_t get_successor_count() const;
    key_t key_init();
    key_t set_key(key_t key);
    // const weight_t get_weight() const;
  protected:
    // const Node<N, E> *get_node_ptr(key_t node_key);
    // friend const Node<N,E> *Graph<N,E>::get_node_ptr(key_t node_key);
    N m_data{nullptr};

  protected:
    const Graph<N, E> *m_graph{nullptr};
    key_t m_key{KEY_UNDEF};
    std::map<key_t, Node<N, E> *> m_successors{};
    std::map<key_t, Node<N, E> *> m_predecessors{};
    ColorT m_color{ColorT::WHITE};
    key_t m_loop{KEY_UNDEF};
};

template <typename N, typename E>
Node<N, E>::Node(N &&data, const Graph<N, E> &g) : m_data(std::move(data)), m_graph(&g) {}

template <typename N, typename E> void Node<N, E>::set_color(ColorT c) { m_color = c; }
template <typename N, typename E> void Node<N, E>::set_loop(key_t head_key) { m_loop = head_key; }
template <typename N, typename E> key_t Node<N, E>::set_key(key_t key) {
    m_key = key;
    return m_key;
}

template <typename N, typename E> ColorT Node<N, E>::get_color() const { return m_color; }
template <typename N, typename E> key_t Node<N, E>::get_loop() const { return m_loop; }

template <typename N, typename E> size_t Node<N, E>::get_successor_count() const {
    return m_successors.size();
}
template <typename N, typename E> size_t Node<N, E>::get_predecessor_count() const {
    return m_predecessors.size();
}

template <typename N, typename E>
typename std::map<key_t, Node<N, E> *>::iterator Node<N, E>::predecessors_begin() {
    return m_predecessors.begin();
}

template <typename N, typename E>
typename std::map<key_t, Node<N, E> *>::iterator Node<N, E>::predecessors_end() {
    return m_predecessors.end();
}

template <typename N, typename E>
typename std::map<key_t, Node<N, E> *>::iterator Node<N, E>::successors_begin() {
    return m_successors.begin();
}

template <typename N, typename E>
typename std::map<key_t, Node<N, E> *>::iterator Node<N, E>::successors_end() {
    return m_successors.end();
}

template <typename N, typename E> key_t Node<N, E>::key_init() {
    if (m_key != KEY_UNDEF) {
        ERROR("Node has already been inited");
        return KEY_UNDEF;
    }
    key_t key = m_graph->get_avail_nd_key();
    m_key = key;
    return m_key;
}

template <typename N, typename E> key_t Node<N, E>::add_successor(key_t s_key) {
    auto find_result = m_graph->m_nodes.find(s_key);
    if (find_result == m_graph->m_nodes.end()) {
        ERROR("Wrong successor key");
        return KEY_UNDEF;
    }
    const Node<N, E> *nptr = find_result->second;
    if (nptr == nullptr) {
        ERROR("Successor search returns NULL");
        return KEY_UNDEF;
    }
    if (!m_successors.insert(std::make_pair(s_key, const_cast<Node<N, E> *>(nptr))).second) {
        LOG("Successor has already existed");
        return KEY_DUBLICATE;
    }
    return s_key;
}

template <typename N, typename E> key_t Node<N, E>::add_predecessor(key_t p_key) {
    auto find_result = m_graph->m_nodes.find(p_key);
    if (find_result == m_graph->m_nodes.end()) {
        ERROR("Wrong predecessor key");
        return KEY_UNDEF;
    }
    const Node<N, E> *nptr = find_result->second;
    if (nptr == nullptr) {
        ERROR("Predecessor search returns NULL");
        return KEY_UNDEF;
    }
    if (!m_predecessors.insert(std::make_pair(p_key, const_cast<Node<N, E> *>(nptr))).second) {
        LOG("Predecessor has already existed");
        return KEY_DUBLICATE;
    }
    return p_key;
}

template <typename N, typename E> key_t Node<N, E>::delete_successor(key_t s_key) {
    auto find_result = m_successors.find(s_key);
    if (find_result == m_successors.end()) {
        ERROR("Wrong successor key");
        return KEY_UNDEF;
    }
    m_successors.erase(s_key);
    return s_key;
}

template <typename N, typename E> key_t Node<N, E>::delete_predecessor(key_t p_key) {
    auto find_result = m_predecessors.find(p_key);
    if (find_result == m_predecessors.end()) {
        ERROR("Wrong predecessor key");
        return KEY_UNDEF;
    }
    m_predecessors.erase(p_key);
    return p_key;
}

template <typename N, typename E> const key_t Node<N, E>::get_key() const { return m_key; }

template <typename N, typename E> N &Node<N, E>::access_data() & { return m_data; }

// N, E - type of additional data for node, edge respectively
template <typename N, typename E> class Edge {
  public:
    Edge(E &&data, const Graph<N, E> &g, key_t start_node_key, key_t end_node_key);
    virtual ~Edge() = default;

    key_t get_key() const;
    key_t get_start_node_key() const;
    key_t get_end_node_key() const;
    key_t key_init();

  public:
    E m_data{};

  protected:
    key_t m_key{KEY_UNDEF};
    key_t m_start_node_key{KEY_UNDEF};
    key_t m_end_node_key{KEY_UNDEF};
    const Graph<N, E> *m_graph{nullptr};
};

template <typename N, typename E>
Edge<N, E>::Edge(E &&data, const Graph<N, E> &g, key_t start_node_key, key_t end_node_key)
    : m_data(std::move(data)), m_start_node_key(start_node_key), m_end_node_key(end_node_key),
      m_graph(&g) {}

template <typename N, typename E> key_t Edge<N, E>::key_init() {
    if (m_key != KEY_UNDEF) {
        ERROR("Node has already been inited");
        return KEY_UNDEF;
    }
    key_t key = m_graph->get_avail_edg_key();
    m_key = key;
    return key;
}

template <typename N, typename E> key_t Edge<N, E>::get_start_node_key() const {
    return m_start_node_key;
}

template <typename N, typename E> key_t Edge<N, E>::get_end_node_key() const {
    return m_end_node_key;
}

template <typename N, typename E> key_t Edge<N, E>::get_key() const { return m_key; }

template <typename N, typename E> class Graph {
  public:
    Graph() = default;
    // visitor
    Graph(const Graph &) = delete;
    Graph(const Graph &&) = delete;
    Graph &operator=(const Graph &) = delete;
    virtual ~Graph();

    virtual key_t add_node(N &&node_data);
    virtual key_t add_node(N &&node_data, key_t key);
    virtual key_t add_nodes(N &&node_data, size_t count);
    virtual key_t delete_node(key_t node_key);
    virtual key_t add_edge(E &&edge_data, key_t start_node_key, key_t end_node_key);
    virtual key_t delete_edge(key_t start_node_key, key_t end_node_key);
    virtual key_t cut_node(key_t node_key);
    virtual key_t cut_edge(key_t start_node_key, key_t end_node_key);
    virtual key_t paste_all();

    Node<N, E> *at(key_t node_key);

    std::vector<key_t> DFS(key_t root_key, key_t end_key = KEY_UNDEF);
    bool hasPath(key_t start, key_t end);
    std::vector<key_t> RPO(key_t root_key);
    std::vector<key_t> getDominatedNodes(key_t root_key, key_t target_node);
    bool is_a_dominates_b(key_t root_key, key_t a, key_t b);
    std::vector<Cycle> get_cycles(key_t root_key, key_t end_key);

    bool node_exists(key_t key) const;
    virtual std::string dump() const;
    key_t get_avail_nd_key() const;
    key_t get_avail_edg_key() const;
    size_t get_node_count() const;
    key_t get_edge_id(key_t start_node_key, key_t end_node_key) const;

    friend key_t Node<N, E>::add_predecessor(key_t p_key);
    friend key_t Node<N, E>::add_successor(key_t s_key);

  protected:
    // only deletes edge
    key_t delete_edge(key_t edge_key);
    key_t cut_edge(key_t edge_key);
    key_t paste_node(key_t node_key);
    key_t paste_edge(key_t edge_key);

  private:
    std::vector<key_t> DFS_(Node<N, E> &nd, key_t end_key);
    std::vector<Cycle> get_cycles_(Node<N, E> &nd, key_t root_key, key_t end_key);
    std::vector<ColorT> preserve_colors_() const;
    void recover_colors_(const std::vector<ColorT> &vec);

  protected:
    std::map<key_t, Node<N, E> *> m_nodes{};
    std::map<key_t, Edge<N, E> *> m_edges{};
    key_t m_actual_node_key{1};
    key_t m_actual_edge_key{1};
    std::map<key_t, Node<N, E> *> m_nodes_buf{};
    std::map<key_t, Edge<N, E> *> m_edges_buf{};
};

template <typename N, typename E> key_t Graph<N, E>::get_avail_nd_key() const {
    return m_actual_node_key;
}

template <typename N, typename E> key_t Graph<N, E>::get_avail_edg_key() const {
    return m_actual_edge_key;
}

template <typename N, typename E> key_t Graph<N, E>::add_node(N &&node_data) {
    Node<N, E> *nptr = new Node<N, E>{std::move(node_data), *this};
    key_t node_key = nptr->key_init();
    auto node_insertion_result = m_nodes.insert(std::make_pair(node_key, nptr));
    if (!node_insertion_result.second) {
        ERROR("Cannot insert node w/ given key");
        delete nptr;
        return KEY_UNDEF;
    }
    m_actual_node_key++;
    return node_key;
}

template <typename N, typename E> key_t Graph<N, E>::add_node(N &&node_data, key_t key) {
    auto node_find_result = m_nodes.find(key);
    if (node_find_result != m_nodes.end()) {
        ERROR("Wrong node key");
        return KEY_UNDEF;
    }
    m_actual_node_key = std::max(key, m_actual_node_key);
    Node<N, E> *nptr = new Node<N, E>{std::move(node_data), *this};
    key_t node_key = nptr->set_key(key);
    auto node_insertion_result = m_nodes.insert(std::make_pair(node_key, nptr));
    if (!node_insertion_result.second) {
        ERROR("Cannot insert node w/ given key");
        delete nptr;
        return KEY_UNDEF;
    }
    m_actual_node_key++;
    return node_key;
}

template <typename N, typename E> key_t Graph<N, E>::add_nodes(N &&node_data, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        auto key = add_node(std::move(node_data));
        if (key == KEY_UNDEF) {
            return KEY_UNDEF;
        }
    }
}

template <typename N, typename E> key_t Graph<N, E>::delete_node(key_t node_key) {
    using edge_item_t = std::pair<key_t, Edge<N, E> *>;
    // find node X by key
    auto node_find_result = m_nodes.find(node_key);
    if (node_find_result == m_nodes.end()) {
        ERROR("Wrong node key");
        return KEY_UNDEF;
    }
    std::vector<key_t> edges_to_delete{};
    for (auto &itm : m_edges) {
        key_t edge_key = itm.first;
        Edge<N, E> *edge = itm.second;
        if (edge->get_end_node_key() == node_key || edge->get_start_node_key() == node_key) {
            edges_to_delete.push_back(edge_key);
        }
    }
    for (auto key : edges_to_delete) {
        delete_edge(key);
    }
    // delete node X
    delete node_find_result->second;
    m_nodes.erase(node_key);
    return node_key;
}

template <typename N, typename E> key_t Graph<N, E>::cut_node(key_t node_key) {
    auto node_find_result = m_nodes.find(node_key);
    if (node_find_result == m_nodes.end()) {
        ERROR("Wrong node key");
        return KEY_UNDEF;
    }
    std::vector<key_t> edges_to_cut{};
    for (auto &itm : m_edges) {
        key_t edge_key = itm.first;
        Edge<N, E> *edge = itm.second;
        if (edge->get_end_node_key() == node_key || edge->get_start_node_key() == node_key) {
            edges_to_cut.push_back(edge_key);
        }
    }
    for (auto key : edges_to_cut) {
        cut_edge(key);
    }

    m_nodes_buf.insert(std::make_pair(node_key, m_nodes[node_key]));
    m_nodes.erase(node_key);
    return node_key;
}

template <typename N, typename E> key_t Graph<N, E>::cut_edge(key_t edge_key) {
    auto result = m_edges.find(edge_key);
    if (result == m_edges.end()) {
        ERROR("No such edge");
        return KEY_UNDEF;
    }

    key_t start_node_key = result->second->get_start_node_key();
    key_t end_node_key = result->second->get_end_node_key();
    if (m_nodes.find(start_node_key) == m_nodes.end() ||
        m_nodes.find(end_node_key) == m_nodes.end()) {
        ERROR("No start/end node - cannot cut edge");
    }

    key_t key1 = m_nodes.at(start_node_key)->delete_successor(end_node_key);
    key_t key2 = m_nodes.at(end_node_key)->delete_predecessor(start_node_key);
    if (key1 == KEY_UNDEF || key2 == KEY_UNDEF) {
        ERROR("Cannot delete Successor/Predecessor during cut");
        return KEY_UNDEF;
    }

    m_edges.erase(edge_key);
    m_edges_buf.insert(std::make_pair(edge_key, result->second));
    return edge_key;
}

template <typename N, typename E>
key_t Graph<N, E>::cut_edge(key_t start_node_key, key_t end_node_key) {
    using edge_item_t = std::pair<key_t, Edge<N, E> *>;
    auto output_node_it = m_nodes.find(start_node_key);
    if (output_node_it == m_nodes.end()) {
        ERROR("Wrong start node key");
        return KEY_UNDEF;
    }
    auto input_node_it = m_nodes.find(end_node_key);
    if (input_node_it == m_nodes.end()) {
        ERROR("Wrong end node key");
        return KEY_UNDEF;
    }
    for (auto &item : m_edges) {
        key_t edge_key = item.first;
        Edge<N, E> *edge = item.second;
        if (edge->get_end_node_key() == end_node_key &&
            edge->get_start_node_key() == start_node_key) {
            return cut_edge(edge_key);
        }
    }
    ERROR("No edges selected for cut");
    return KEY_UNDEF;
}

template <typename N, typename E>
key_t Graph<N, E>::get_edge_id(key_t start_node_key, key_t end_node_key) const {
    using edge_item_t = std::pair<key_t, Edge<N, E> *>;
    auto output_node_it = m_nodes.find(start_node_key);
    if (output_node_it == m_nodes.end()) {
        ERROR("Wrong start node key");
        return KEY_UNDEF;
    }
    auto input_node_it = m_nodes.find(end_node_key);
    if (input_node_it == m_nodes.end()) {
        ERROR("Wrong end node key");
        return KEY_UNDEF;
    }
    for (auto &item : m_edges) {
        key_t edge_key = item.first;
        Edge<N, E> *edge = item.second;
        if (edge->get_end_node_key() == end_node_key &&
            edge->get_start_node_key() == start_node_key) {
            return edge_key;
        }
    }
    ERROR("No edges selected for cut");
    return KEY_UNDEF;
}

template <typename N, typename E>
key_t Graph<N, E>::add_edge(E &&edge_data, key_t start_node_key, key_t end_node_key) {
    // check if nodes exist
    auto start_node_it = m_nodes.find(start_node_key);
    if (start_node_it == m_nodes.end()) {
        ERROR("Wrong start node key");
        return KEY_UNDEF;
    }
    auto end_node_it = m_nodes.find(end_node_key);
    if (end_node_it == m_nodes.end()) {
        ERROR("Wrong end node key");
        return KEY_UNDEF;
    }

    key_t key1 = start_node_it->second->add_successor(end_node_key);
    key_t key2 = end_node_it->second->add_predecessor(start_node_key);

    if (key1 == KEY_DUBLICATE && key2 == KEY_DUBLICATE) {
        LOG("Such edge has already exists");
        return KEY_DUBLICATE;
    } else if (key1 == KEY_DUBLICATE || key2 == KEY_DUBLICATE || key1 == KEY_UNDEF ||
               key2 == KEY_UNDEF) {
        ERROR("Successor & Predecessor have corrupted data");
        return KEY_UNDEF;
    }

    Edge<N, E> *eptr = new Edge<N, E>{std::move(edge_data), *this, start_node_key, end_node_key};
    key_t edge_key = eptr->key_init();
    if (!m_edges.insert(std::make_pair(edge_key, eptr)).second) {
        ERROR("Cannot insert edge w/ given key");
        delete eptr;
        return KEY_UNDEF;
    }
    m_actual_edge_key++;
    return edge_key;
}

#if 0
template <typename N, typename E>
key_t Graph<N, E>::paste_edge(key_t start_node_key, key_t end_node_key) {
    // check if nodes exist
    auto start_node_it = m_nodes.find(start_node_key);
    if (start_node_it == m_nodes.end()) {
        ERROR("Wrong start node key");
        return KEY_UNDEF;
    }
    auto end_node_it = m_nodes.find(end_node_key);
    if (end_node_it == m_nodes.end()) {
        ERROR("Wrong end node key");
        return KEY_UNDEF;
    }

    // check if edge in buffer
    key_t edge_key{KEY_UNDEF};
    for (auto &item: m_edges_buf) {
        Edge<N, E> *edge = item.second;
        if (edge->get_start_node_key() == start_node_key && edge->get_end_node_key() == end_node_key) {
            edge_key = item.first;
        }
    }
    if (edge_key == KEY_UNDEF) {
        ERROR("No such edge in buffer");
        return KEY_UNDEF
    }
    return paste_edge(edge_key);
}
#endif

template <typename N, typename E> key_t Graph<N, E>::paste_edge(key_t edge_key) {
    auto edge_it = m_edges_buf.find(edge_key);
    if (edge_it == m_edges_buf.end()) {
        ERROR("No such edge in buf");
        return KEY_UNDEF;
    }
    if (m_edges.find(edge_key) != m_edges.end()) {
        ERROR("Edge cannot be pasted");
        return KEY_UNDEF;
    }
    key_t start_node_key = edge_it->second->get_start_node_key();
    key_t end_node_key = edge_it->second->get_end_node_key();
    if (m_nodes.find(start_node_key) == m_nodes.end() ||
        m_nodes.find(end_node_key) == m_nodes.end()) {
        ERROR("No nodes for pasted edge");
        return KEY_UNDEF;
    }
    m_edges.insert(std::make_pair(edge_it->first, edge_it->second));
    m_edges_buf.erase(edge_key);
    Node<N, E> *start_node = m_nodes.at(start_node_key);
    Node<N, E> *end_node = m_nodes.at(end_node_key);
    key_t key1 = start_node->add_successor(end_node_key);
    key_t key2 = end_node->add_predecessor(start_node_key);
    if (key1 == KEY_UNDEF || key2 == KEY_UNDEF) {
        ERROR("Cannot delete Successor/Predecessor during paste");
    }
    return edge_key;
}

template <typename N, typename E> key_t Graph<N, E>::paste_node(key_t node_key) {
    auto node_find_result = m_nodes_buf.find(node_key);
    if (node_find_result == m_nodes_buf.end()) {
        ERROR("Wrong pasted node key");
        return KEY_UNDEF;
    }

    if (m_nodes.find(node_key) != m_nodes.end()) {
        ERROR("Cannot paste node - already exists in node list");
        return KEY_UNDEF;
    }

    m_nodes_buf.erase(node_key);
    m_nodes.insert(std::make_pair(node_find_result->first, node_find_result->second));

    return node_key;
}

template <typename N, typename E> key_t Graph<N, E>::paste_all() {
    while (m_nodes_buf.size()) {
        key_t key = m_nodes_buf.rbegin()->first;
        if (paste_node(key) == KEY_UNDEF) {
            ERROR("Error during pasting node");
            return KEY_UNDEF;
        }
    }
    while (m_edges_buf.size()) {
        key_t key = m_edges_buf.rbegin()->first;
        if (paste_edge(key) == KEY_UNDEF) {
            ERROR("Error during pasting edge");
            return KEY_UNDEF;
        }
    }
    return 1;
}

template <typename N, typename E>
key_t Graph<N, E>::delete_edge(key_t start_node_key, key_t end_node_key) {
    using edge_item_t = std::pair<key_t, Edge<N, E> *>;
    auto output_node_it = m_nodes.find(start_node_key);
    if (output_node_it == m_nodes.end()) {
        ERROR("Wrong start node key");
        return KEY_UNDEF;
    }
    auto input_node_it = m_nodes.find(end_node_key);
    if (input_node_it == m_nodes.end()) {
        ERROR("Wrong end node key");
        return KEY_UNDEF;
    }
    for (auto &item : m_edges) {
        key_t edge_key = item.first;
        Edge<N, E> *edge = item.second;
        if (edge->get_end_node_key() == end_node_key &&
            edge->get_start_node_key() == start_node_key) {
            key_t key1 = m_nodes.at(start_node_key)->delete_successor(end_node_key);
            key_t key2 = m_nodes.at(end_node_key)->delete_predecessor(start_node_key);
            if (key1 == KEY_UNDEF || key2 == KEY_UNDEF) {
                ERROR("Cannot delete Successor / Predecessor");
                return KEY_UNDEF;
            }
            return delete_edge(edge_key);
        }
    }
    ERROR("No edges selected");
    return KEY_UNDEF;
}

template <typename N, typename E> key_t Graph<N, E>::delete_edge(key_t edge_key) {
    auto result = m_edges.find(edge_key);
    if (result == m_edges.end()) {
        ERROR("No such edge");
        return KEY_UNDEF;
    }
    m_edges.erase(edge_key);
    delete result->second;
    return edge_key;
}

template <typename N, typename E> Node<N, E> *Graph<N, E>::at(key_t key) {
    auto res = m_nodes.find(key);
    if (res == m_nodes.end()) {
        ERROR("No such node key");
        return nullptr;
    }
    return res->second;
}

template <typename N, typename E> bool Graph<N, E>::node_exists(key_t key) const {
    if (m_nodes.find(key) == m_nodes.end()) {
        return false;
    }
    return true;
}

template <typename N, typename E> size_t Graph<N, E>::get_node_count() const {
    return m_nodes.size();
}

template <typename N, typename E> Graph<N, E>::~Graph() {
    for (auto &node_item : m_nodes) {
        auto node_ptr = node_item.second;
        delete node_ptr;
    }
    for (auto &edge_item : m_edges) {
        auto edge_ptr = edge_item.second;
        delete edge_ptr;
    }
    for (auto &node_item : m_nodes_buf) {
        auto node_ptr = node_item.second;
        delete node_ptr;
    }
    for (auto &edge_item : m_edges_buf) {
        auto edge_ptr = edge_item.second;
        delete edge_ptr;
    }
}

template <typename N, typename E> std::string Graph<N, E>::dump() const {
    std::stringstream ss{};
    ss << "digraph G {\n";
    for (auto &item : m_nodes) {
        ss << "\t" << item.first << "\n";
    }
    for (auto &item : m_edges) {
        key_t edge_key = item.first;
        Edge<N, E> *edge = item.second;
        ss << "\t" << edge->get_start_node_key() << " -> " << edge->get_end_node_key() << "\t";
        ss << "[label=" << edge_key << "]\n";
    }
    ss << "}\n";
    return ss.str();
}

template <typename N, typename E>
std::vector<key_t> Graph<N, E>::DFS(key_t root_key, key_t end_key) {
    auto res = m_nodes.find(root_key);
    if (res == m_nodes.end()) {
        ERROR("No such node key");
        return {};
    }
    std::vector<ColorT> colors = preserve_colors_();
    auto *node = res->second;
    std::vector<key_t> vec{};
    if (node->get_color() == ColorT::WHITE) {
        vec = DFS_(*node, end_key);
    }
    recover_colors_(colors);
    return vec;
}

template <typename N, typename E>
std::vector<key_t> Graph<N, E>::DFS_(Node<N, E> &nd, key_t end_key) {
    std::vector<key_t> vec{nd.get_key()};
    if (nd.get_key() == end_key) {
        nd.set_color(ColorT::BLACK);
        return vec;
    }
    nd.set_color(ColorT::GRAY);
    for (auto it = nd.successors_begin(); it != nd.successors_end(); it++) {
        if (it->second->get_color() == ColorT::WHITE) {
            std::vector<key_t> tmp = DFS_(*(it->second), end_key);
            vec.insert(vec.end(), tmp.begin(), tmp.end());
            if (vec.back() == end_key) {
                nd.set_color(ColorT::BLACK);
                return vec;
            }
        }
    }
    nd.set_color(ColorT::BLACK);
    return vec;
}

template <typename N, typename E>
std::vector<Cycle> Graph<N, E>::get_cycles(key_t root_key, key_t end_key) {
    auto res = m_nodes.find(root_key);
    if (res == m_nodes.end()) {
        ERROR("No such node key");
        return {};
    }
    std::vector<ColorT> colors = preserve_colors_();
    auto *node = res->second;
    std::vector<Cycle> vec{};
    if (node->get_color() == ColorT::WHITE) {
        vec = get_cycles_(*node, root_key, end_key);
    }
    recover_colors_(colors);
    for (auto &cycle : vec) {
        if (is_a_dominates_b(root_key, cycle.head_node, cycle.back_edge_start)) {
            cycle.reducible = true;
        }
    }
    // recover_colors_(colors);
    return vec;
}

template <typename N, typename E>
std::vector<Cycle> Graph<N, E>::get_cycles_(Node<N, E> &nd, key_t root_key, key_t end_key) {
    std::vector<Cycle> vec{};
    // if (nd.get_key() == end_key) {
    //     nd.set_color(ColorT::BLACK);
    //     return vec;
    // }
    nd.set_color(ColorT::GRAY);
    for (auto it = nd.successors_begin(); it != nd.successors_end(); it++) {
        if (it->second->get_color() == ColorT::GRAY) {
            key_t backedge_end = it->second->get_key();
            key_t backedge_start = nd.get_key();
            key_t edge_key = get_edge_id(backedge_start, backedge_end);
            key_t head_key = backedge_end;
            // bool reducible = !is_a_dominates_b(root_key, backedge_end, backedge_start);
            Cycle c = {head_key, edge_key, backedge_start, false};
            vec.push_back(c);
        }
        if (it->second->get_color() == ColorT::WHITE) {
            std::vector<Cycle> tmp = get_cycles_(*(it->second), root_key, end_key);
            vec.insert(vec.end(), tmp.begin(), tmp.end());
            // if (vec.back() == end_key) {
            //     nd.set_color(ColorT::BLACK);
            //     return vec;
            // }
        }
    }
    nd.set_color(ColorT::BLACK);
    return vec;
}

template <typename N, typename E> std::vector<ColorT> Graph<N, E>::preserve_colors_() const {
    std::vector<ColorT> vec{};
    for (auto &it : m_nodes) {
        vec.push_back(it.second->get_color());
    }
    return vec;
}

template <typename N, typename E>
void Graph<N, E>::recover_colors_(const std::vector<ColorT> &vec) {
    if (vec.size() != m_nodes.size()) {
        ERROR("Wrong countity of colors");
        return;
    }

    auto i = 0;
    for (auto &it : m_nodes) {
        it.second->set_color(vec[i++]);
    }
}

template <typename N, typename E> std::vector<key_t> Graph<N, E>::RPO(key_t root_key) {
    auto res = m_nodes.find(root_key);
    if (res == m_nodes.end()) {
        ERROR("No such node key");
        return {};
    }
    std::vector<key_t> vec{};
    auto *node = res->second;
    node->set_color(ColorT::BLACK);
    std::queue<key_t> q{};
    q.push(node->get_key());
    while (!q.empty()) {
        key_t key = q.front();
        vec.push_back(key);
        q.pop();
        Node<N, E> *node = m_nodes[key];
        for (auto it = node->successors_begin(); it != node->successors_end(); it++) {
            Node<N, E> *nd = it->second;
            if (nd->get_color() != ColorT::BLACK) {
                // vec.push_back(nd->get_key());
                nd->set_color(ColorT::BLACK);
                q.push(nd->get_key());
            }
        }
    }
    for (auto &it : m_nodes) {
        it.second->set_color(ColorT::WHITE);
    }
    return vec;
}

// not tested yet
template <typename N, typename E>
std::vector<key_t> Graph<N, E>::getDominatedNodes(key_t root_key, key_t target_key) {
    auto res = m_nodes.find(root_key);
    if (res == m_nodes.end()) {
        ERROR("No such root key");
        return std::vector<key_t>{KEY_UNDEF};
    }
    res = m_nodes.find(target_key);
    if (res == m_nodes.end()) {
        ERROR("No such target key");
        return std::vector<key_t>{KEY_UNDEF};
    }
    std::vector<key_t> origin = DFS(root_key);
    // for (auto x: origin) {
    //     std::cerr << x << " ";
    // }
    // std::cerr << "\n";
    // std::set<key_t> initial_nodes(tmp.begin(), tmp.end());
    cut_node(target_key);
    std::vector<key_t> reduced = DFS(root_key);
    // for (auto x: reduced) {
    //     std::cerr << x << " ";
    // }
    // std::cerr << "\n";
    for (auto node_key : reduced) {
        for (auto it = origin.begin(); it != origin.end(); ++it) {
            if (*it == node_key) {
                origin.erase(it);
                break;
            }
        }
    }
    paste_all();
    return origin;
}

template <typename N, typename E>
bool Graph<N, E>::is_a_dominates_b(key_t root_key, key_t a, key_t b) {
    auto employees = getDominatedNodes(root_key, a);
    for (auto &key : employees) {
        if (b == key) {
            return true;
        }
    }
    return false;
}

template <typename N, typename E> bool Graph<N, E>::hasPath(key_t start, key_t end) {
    auto path = DFS(start, end);
    if (path.size() == 0 || path.back() != end) {
        return false;
    }
    return true;
}

} // namespace G
