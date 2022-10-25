#pragma once
#include "config.hpp"
#include <exception>
#include <list>
#include <map>
#include <optional>
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
};

using weight_t = int;

template <typename N, typename E> class Graph;

// N - type of additional data
template <typename N, typename E> class Node {

  public:
    Node(const N &&data, const Graph<N, E> &g);
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
    N &access_data() &;
    const key_t get_key() const;
    ColorT get_color() const;
    key_t key_init();
    // const weight_t get_weight() const;
  protected:
    // const Node<N, E> *get_node_ptr(key_t node_key);
    // friend const Node<N,E> *Graph<N,E>::get_node_ptr(key_t node_key);
    N m_data{};

  protected:
    const Graph<N, E> *m_graph{nullptr};
    key_t m_key{KEY_UNDEF};
    std::map<key_t, Node<N, E> *> m_successors{};
    std::map<key_t, Node<N, E> *> m_predecessors{};
    ColorT m_color{ColorT::WHITE};
};

template <typename N, typename E>
Node<N, E>::Node(const N &&data, const Graph<N, E> &g) : m_data(std::move(data)), m_graph(&g) {}

template <typename N, typename E> void Node<N, E>::set_color(ColorT c) { m_color = c; }

template <typename N, typename E> ColorT Node<N, E>::get_color() const { return m_color; }

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
    Edge(E data, const Graph<N, E> &g, key_t start_node_key, key_t end_node_key);
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
Edge<N, E>::Edge(E data, const Graph<N, E> &g, key_t start_node_key, key_t end_node_key)
    : m_data(data), m_start_node_key(start_node_key), m_end_node_key(end_node_key), m_graph(&g) {}

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

    virtual key_t add_node(const N &&node_data);
    virtual key_t delete_node(key_t node_key);
    virtual key_t add_edge(E edge_data, key_t start_node_key, key_t end_node_key);
    virtual key_t delete_edge(key_t start_node_key, key_t end_node_key);
    std::optional<Node<N, E> &> access_node(key_t node_key);

    std::vector<key_t> DFS(key_t root_key);

    bool node_exists(key_t key) const;
    virtual std::string dump() const;
    key_t get_avail_nd_key() const;
    key_t get_avail_edg_key() const;

    friend key_t Node<N, E>::add_predecessor(key_t p_key);
    friend key_t Node<N, E>::add_successor(key_t s_key);

  protected:
    // only deletes edge
    key_t delete_edge(key_t edge_key);

  private:
    std::vector<key_t> DFS_(Node<N, E> &nd);

  protected:
    std::map<key_t, Node<N, E> *> m_nodes{};
    std::map<key_t, Edge<N, E> *> m_edges{};
    key_t m_actual_node_key{1};
    key_t m_actual_edge_key{1};
};

template <typename N, typename E> key_t Graph<N, E>::get_avail_nd_key() const {
    return m_actual_node_key;
}

template <typename N, typename E> key_t Graph<N, E>::get_avail_edg_key() const {
    return m_actual_edge_key;
}

template <typename N, typename E> key_t Graph<N, E>::add_node(const N &&node_data) {
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

template <typename N, typename E> key_t Graph<N, E>::delete_node(key_t node_key) {
    using edge_item_t = std::pair<key_t, Edge<N, E> *>;
    // find node X by key
    auto node_find_result = m_nodes.find(node_key);
    if (node_find_result == m_nodes.end()) {
        ERROR("Wrong node key");
        return KEY_UNDEF;
    }
    for (auto &itm : m_edges) {
        key_t edge_key = itm.first;
        Edge<N, E> *edge = itm.second;
        if (edge->get_end_node_key() == node_key || edge->get_start_node_key() == node_key) {
            delete_edge(edge_key);
        }
    }
    // delete node X
    delete node_find_result->second;
    m_nodes.erase(node_key);
    return node_key;
}

template <typename N, typename E>
key_t Graph<N, E>::add_edge(E edge_data, key_t start_node_key, key_t end_node_key) {
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

    Edge<N, E> *eptr = new Edge<N, E>{edge_data, *this, start_node_key, end_node_key};
    key_t edge_key = eptr->key_init();
    if (!m_edges.insert(std::make_pair(edge_key, eptr)).second) {
        ERROR("Cannot insert edge w/ given key");
        delete eptr;
        return KEY_UNDEF;
    }
    m_actual_edge_key++;
    return edge_key;
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

template <typename N, typename E> std::optional<Node<N, E> &> Graph<N, E>::access_node(key_t key) {
    auto res = m_nodes.find(key);
    if (res == m_nodes.end()) {
        ERROR("No such node key");
        return std::nullopt;
    }
    return {*res->second};
}

template <typename N, typename E> bool Graph<N, E>::node_exists(key_t key) const {
    if (m_nodes.find(key) == m_nodes.end()) {
        return false;
    }
    return true;
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
        ss << "\t" << edge->get_start_node_key() << " -> " << edge->get_end_node_key() << "\n";
    }
    ss << "}\n";
    return ss.str();
}

template <typename N, typename E> std::vector<key_t> Graph<N, E>::DFS(key_t root_key) {
    auto res = m_nodes.find(root_key);
    if (res == m_nodes.end()) {
        ERROR("No such node key");
        return {};
    }
    auto *node = res->second;
    std::vector<key_t> vec{};
    if (node->get_color() == ColorT::WHITE) {
        vec = DFS_(*node);
    }
    for (auto &it : m_nodes) {
        it.second->set_color(ColorT::WHITE);
    }
    return vec;
}

template <typename N, typename E> std::vector<key_t> Graph<N, E>::DFS_(Node<N, E> &nd) {
    std::vector<key_t> vec{nd.get_key()};
    nd.set_color(ColorT::GRAY);
    for (auto it = nd.successors_begin(); it != nd.successors_end(); it++) {
        if (it->second->get_color() == ColorT::WHITE) {
            std::vector<key_t> tmp = DFS_(*(it->second));
            vec.insert(vec.end(), tmp.begin(), tmp.end());
        }
    }
    nd.set_color(ColorT::BLACK);
    return vec;
}

} // namespace G
