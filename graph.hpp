#pragma once
#include "config.hpp"
#include <exception>
#include <string>
#include <list>
#include <map>
#include <set>
#include <sstream>

namespace G {
using key_t = unsigned long long;
constexpr key_t KEY_UNDEF = 0;
using weight_t = int;

template <typename N, typename E> class Graph;

// N - type of additional data
template <typename N, typename E> class Node {
  public:
    Node(N data, key_t node_key, const Graph<N, E> &g);
    virtual ~Node(){};
    virtual key_t add_successor(key_t s_key);
    virtual key_t add_predecessor(key_t p_key);
    virtual key_t delete_successor(key_t s_key);
    virtual key_t delete_predecessor(key_t p_key);
    const key_t get_key() const;
    // const weight_t get_weight() const;
  protected:
    // const Node<N, E> *get_node_ptr(key_t node_key);
    // friend const Node<N,E> *Graph<N,E>::get_node_ptr(key_t node_key);
  public:
    N m_data{};

  protected:
    const Graph<N, E> *m_graph{nullptr};
    key_t m_key{KEY_UNDEF};

    // must be atomic
    static std::set<key_t> m_keys;

    using neighbours_t = std::map<key_t, Node<N, E> *>;
    std::map<key_t, Node<N, E> *> m_successors{};
    std::map<key_t, Node<N, E> *> m_predecessors{};
};

template <typename N, typename E> std::set<key_t> Node<N, E>::m_keys{};

template <typename N, typename E>
Node<N, E>::Node(N data, key_t node_key, const Graph<N, E> &g) : m_data(data), m_graph(&g) {
    auto find_result = m_keys.find(node_key);
    if (find_result != m_keys.end() || node_key == KEY_UNDEF) {
        throw std::logic_error("invalid node key");
    } else {
        m_keys.insert(node_key);
        m_key = node_key;
    }
}

// template <typename N, typename E> const Node<N, E> *Node<N, E>::get_node_ptr(key_t node_key) {
//     auto find_result = m_graph->m_nodes.find(node_key);
//     if (find_result == m_graph->m_nodes.end()) {
//         ERROR("wrong ptr");
//         return nullptr;
//     }
//     return find_result->second;
// }

template <typename N, typename E> key_t Node<N, E>::add_successor(key_t s_key) {
    auto find_result = m_graph->m_nodes.find(s_key);
    if (find_result == m_graph->m_nodes.end()) {
        ERROR("wrong ptr");
        return KEY_UNDEF;
    }
    const Node<N, E> *nptr = find_result->second;
    if (nptr == nullptr) {
        ERROR("wrong key");
        return KEY_UNDEF;
    }
    m_successors.insert(std::make_pair(s_key, const_cast<Node<N, E> *>(nptr)));

    return s_key;
}

template <typename N, typename E> key_t Node<N, E>::add_predecessor(key_t p_key) {
    auto find_result = m_graph->m_nodes.find(p_key);
    if (find_result == m_graph->m_nodes.end()) {
        ERROR("wrong ptr");
        return KEY_UNDEF;
    }
    const Node<N, E> *nptr = find_result->second;
    if (nptr == nullptr) {
        ERROR("wrong key");
        return KEY_UNDEF;
    }
    m_predecessors.insert(std::make_pair(p_key, const_cast<Node<N, E> *>(nptr)));

    return p_key;
}

template <typename N, typename E> key_t Node<N, E>::delete_predecessor(key_t p_key) {
    auto find_result = m_predecessors.find(p_key);
    if (find_result == m_predecessors.end()) {
        ERROR("wrong ptr");
        return KEY_UNDEF;
    }
    
    m_predecessors.erase(p_key);

    return p_key;
}

template <typename N, typename E> key_t Node<N, E>::delete_successor(key_t s_key) {
    auto find_result = m_successors.find(s_key);
    if (find_result == m_successors.end()) {
        ERROR("wrong ptr");
        return KEY_UNDEF;
    }
    
    m_successors.erase(s_key);

    return s_key;
}

template <typename N, typename E> const key_t Node<N, E>::get_key() const { return m_key; }

// N, E - type of additional data for node, edge respectively
template <typename N, typename E> class Edge {
  public:
    Edge(E data, key_t start_node_key, key_t end_node_key, key_t edge_key);
    virtual ~Edge(){};

    // const E& get_data() const;
    key_t get_key() const;
    key_t get_start_node_key() const;
    key_t get_end_node_key() const;
    // const weight_t get_weight() const;

  public:
    E m_data{};

  protected:
    key_t m_key{KEY_UNDEF};
    key_t m_start_node_key{KEY_UNDEF};
    key_t m_end_node_key{KEY_UNDEF};

    // must be atomic
    static std::set<key_t> m_keys;
};

template <typename N, typename E>
key_t Edge<N,E>::get_start_node_key() const {
    return m_start_node_key;
}

template <typename N, typename E>
key_t Edge<N,E>::get_end_node_key() const {
    return m_end_node_key;
}

template <typename N, typename E>
Edge<N, E>::Edge(E data, key_t start_node_key, key_t end_node_key, key_t edge_key)
    : m_data(data), m_start_node_key(start_node_key), m_end_node_key(end_node_key) {

    auto find_result = m_keys.find(edge_key);
    if (find_result != m_keys.end() || edge_key == KEY_UNDEF) {
        throw std::logic_error("invalid edge key");
    } else {
        m_keys.insert(edge_key);
        m_key = edge_key;
    }
}

template <typename N, typename E> key_t Edge<N, E>::get_key() const { return m_key; }

template <typename N, typename E> std::set<key_t> Edge<N, E>::m_keys{};

template <typename N, typename E> class Graph {
  public:
    Graph() = default;
    // visitor
    Graph(const Graph &) = delete;
    Graph(const Graph &&) = delete;
    Graph &operator=(const Graph &) = delete;
    virtual ~Graph() = default;

    virtual key_t add_node(N node_data, key_t node_key);
    virtual key_t delete_node(key_t node_key);
    virtual key_t add_edge(E edge_data, key_t start_node_key, key_t end_node_key, key_t edge_key);
    virtual key_t delete_edge(key_t start_node_key, key_t end_node_key, key_t edge_key);
    // const Node &
    // access_node(key_t node_key) const &; const Edge& access_edge(key_t
    // edge_key) const &;
    bool node_exists(key_t key) const;
    virtual std::string dump() const;

    // const Node<N,E>* get_node_ptr(key_t node_key);
    friend key_t Node<N, E>::add_successor(key_t s_key);
    friend key_t Node<N, E>::add_predecessor(key_t p_key);

  protected:
    std::map<key_t, Node<N, E> *> m_nodes{};

    using neighbours_t = std::map<key_t, std::list<Edge<N, E> *>>;
    // матрица смежности для предшественников (ключ вершины;указатель на ребро)
    neighbours_t m_inputs{};
    // матрица смежности для последователей (ключ вершины;указатель на ребро)
    neighbours_t m_outputs{};
};

template <typename N, typename E>
std::string Graph<N, E>::dump() const {
    std::stringstream ss{};
    ss << "digraph G {\n";
    
    for (auto &item: m_nodes) {
        ss << "\t" << item.first << "\n";
    }
    
    for (auto &item: m_inputs) {
        auto node_key = item.first;
        auto list = item.second;
        for (auto &edge: list) {
            auto output_node_key = edge->get_start_node_key();
            ss << "\t" << output_node_key << " -> " << node_key << "\n";
        }
    }

    ss << "\n";

    for (auto &item: m_outputs) {
        auto node_key = item.first;
        auto list = item.second;
        for (auto &edge: list) {
            auto input_node_key = edge->get_end_node_key();
            ss << "\t" << node_key << " -> " << input_node_key << "\n";
        }
    }
    ss << "}\n";

    return ss.str();
}

template <typename N, typename E> key_t Graph<N, E>::add_node(N node_data, key_t node_key) {
    Node<N, E> *nptr = new Node<N, E>{node_data, node_key, *this};
    if (nptr == nullptr) {
        throw std::logic_error("cannot allocate node");
    }

    auto node_insertion_result = m_nodes.insert(std::make_pair(node_key, nptr));
    if (!node_insertion_result.second) {
        throw std::logic_error("cannot add node");
    }

    auto edge_init_result = m_inputs.insert(std::make_pair(node_key, std::list<Edge<N, E> *>{}));
    if (!edge_init_result.second) {
        m_nodes.erase(node_key);
        throw std::logic_error("cannot insert predecessor");
    }
    edge_init_result = m_outputs.insert(std::make_pair(node_key, std::list<Edge<N, E> *>{}));
    if (!edge_init_result.second) {
        m_nodes.erase(node_key);
        m_inputs.erase(node_key);
        throw std::logic_error("cannot insert successor");
    }

    return node_key;
}

template <typename N, typename E> key_t Graph<N, E>::delete_node(key_t node_key) {
    auto node_find_result = m_nodes.find(node_key);
    if (node_find_result == m_nodes.end()) {
        ERROR("wrong node key");
        return KEY_UNDEF;
    }

    if (m_inputs.find(node_key) != m_inputs.end()) {
        // delete edges
        m_inputs.erase(node_key);
    } else {
        LOG("node deleition: no input edges detected");
    }
    if (m_outputs.find(node_key) != m_outputs.end()) {
        // not delete edges (has just deleted)
        m_outputs.erase(node_key);
    } else {
        LOG("node deletion: no output edges detected");
    }
    delete node_find_result->second;
    return node_key;
}

template <typename N, typename E>
key_t Graph<N, E>::add_edge(E edge_data, key_t start_node_key, key_t end_node_key, key_t edge_key) {
    // check if nodes exist
    auto start_node_it = m_nodes.find(start_node_key);
    if (start_node_it == m_nodes.end()) {
        ERROR("wrong start node key");
        return KEY_UNDEF;
    }
    auto end_node_it = m_nodes.find(end_node_key);
    if (end_node_it == m_nodes.end()) {
        ERROR("wrong end node key");
        return KEY_UNDEF;
    }

    Edge<N, E> *eptr = new Edge<N, E>{edge_data, start_node_key, end_node_key, edge_key};
    if (eptr == nullptr) {
        throw std::logic_error("cannot allocate edge");
    }

    start_node_it->second->add_successor(end_node_key);
    end_node_it->second->add_predecessor(start_node_key);

    m_inputs.find(end_node_key)->second.push_back(eptr);
    m_outputs.find(start_node_key)->second.push_back(eptr);

    return edge_key;
}

template <typename N, typename E>
key_t Graph<N, E>::delete_edge(key_t start_node_key, key_t end_node_key, key_t edge_key) {
    auto output_edges_list_it = m_outputs.find(start_node_key);
    if (output_edges_list_it == m_outputs.end()) {
        ERROR("wrong start node key");
        return KEY_UNDEF;
    }
    auto &output_edges_list = output_edges_list_it->second;
    auto edge_it1 = output_edges_list.begin();
    for (edge_it1 = output_edges_list.begin(); edge_it1 != output_edges_list.end(); ++edge_it1) {
        if ((*edge_it1)->get_key() == edge_key) {
            // output_edges_list.erase(edge_it1);
            break;
        }
    }
    if (edge_it1 == output_edges_list.end()) {
        throw std::logic_error("edge doesn't contain start node");
    }

    auto input_edges_list_it = m_inputs.find(end_node_key);
    if (input_edges_list_it == m_inputs.end()) {
        ERROR("wrong end node key");
        return KEY_UNDEF;
    }
    auto &input_edges_list = input_edges_list_it->second;
    auto edge_it2 = input_edges_list.begin();
    for (edge_it2 = input_edges_list.begin(); edge_it2 != input_edges_list.end(); ++edge_it2) {
        if ((*edge_it2)->get_key() == edge_key) {
            // input_edges_list.erase(edge_it2);
            break;
        }
    }
    if (edge_it2 == input_edges_list.end()) {
        throw std::logic_error("edge doesn't contain end node");
    }

    if (*edge_it1 != *edge_it2) {
        throw std::logic_error("edge key has > 1 ptrs");
    }

    output_edges_list.erase(edge_it1);
    input_edges_list.erase(edge_it2);
    delete *edge_it1;

    return edge_key;
}

template <typename N, typename E> bool Graph<N, E>::node_exists(key_t key) const {
    if (m_nodes.find(key) == m_nodes.end()) {
        return false;
    }
    return true;
}

} // namespace G
