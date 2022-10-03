#include <exception>
#include <list>
#include <map>
#include <set>

namespace G {
using key_t = unsigned long long;
constexpr key_t KEY_UNDEF = 0;
using weight_t = int;

template <typename N, typename E> class Graph;

// N - type of additional data
template <typename N, typename E> class Node {
  public:
    Node(N data, key_t node_key);
    virtual ~Node(){};
    virtual key_t add_successor(const Node<N, E> *nptr, const Graph<N, E> &g);
    virtual key_t add_predecessor(const Node<N, E> *nptr, const Graph<N, E> &g);

    // const N& get_data() const;
    const key_t get_key() const;
    // const weight_t get_weight() const;

  private:
    key_t m_key{KEY_UNDEF};
    N m_data{};

    // must be atomic
    static std::set<key_t> m_keys;

    using neighbours_t = std::map<key_t, Node<N, E> *>;
    std::map<key_t, Node<N, E> *> m_successors{};
    std::map<key_t, Node<N, E> *> m_predecessors{};
};

template <typename N, typename E> std::set<key_t> Node<N, E>::m_keys{};

template <typename N, typename E> Node<N, E>::Node(N data, key_t node_key) : m_data(data) {
    auto find_result = m_keys.find(node_key);
    if (find_result != m_keys.end() || node_key == KEY_UNDEF) {
        throw std::logic_error("invalid node key");
    } else {
        m_keys.insert(node_key);
        m_key = node_key;
    }
}

template <typename N, typename E>
key_t Node<N, E>::add_successor(const Node<N, E> *nptr, const Graph<N, E> &g) {
    if (nptr == nullptr || (sizeof(*nptr) != sizeof(Node<N, E>))) {
        return KEY_UNDEF;
    }
    const auto key = nptr->get_key();
    if (!g.node_exists(key)) {
        return KEY_UNDEF;
    }
    m_successors.insert(std::make_pair(key, const_cast<Node<N, E> *>(nptr)));

    return key;
}

template <typename N, typename E>
key_t Node<N, E>::add_predecessor(const Node<N, E> *nptr, const Graph<N, E> &g) {
    if (nptr == nullptr || (sizeof(*nptr) != sizeof(Node<N, E>))) {
        return KEY_UNDEF;
    }
    const auto key = nptr->get_key();
    if (!g.node_exists(key)) {
        return KEY_UNDEF;
    }
    m_predecessors.insert(std::make_pair(key, const_cast<Node<N, E> *>(nptr)));
    return key;
}

template <typename N, typename E> const key_t Node<N, E>::get_key() const { return m_key; }

// N, E - type of additional data for node, edge respectively
template <typename N, typename E> class Edge {
  public:
    Edge(E data, key_t start_node_key, key_t end_node_key, key_t edge_key = KEY_UNDEF);
    virtual ~Edge(){};

    // const E& get_data() const;
    key_t get_key() const;
    // const weight_t get_weight() const;
  private:
    key_t m_key{KEY_UNDEF};
    key_t m_start_node_key{KEY_UNDEF};
    key_t m_end_node_key{KEY_UNDEF};
    E m_data{};

    // must be atomic
    static std::set<key_t> m_keys;
};

template <typename N, typename E>
Edge<N, E>::Edge(E data, key_t start_node_key, key_t end_node_key, key_t edge_key)
    : m_start_node_key(start_node_key), m_end_node_key(end_node_key), m_data(data) {

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
    Graph(const Graph &) = delete;
    Graph(const Graph &&) = delete;
    Graph &operator=(const Graph &) = delete;

    key_t add_node(N node_data, key_t node_key = 0);
    key_t delete_node(key_t node_key);
    key_t add_edge(E edge_data, key_t start_node_key, key_t end_node_key,
                   key_t edge_key = KEY_UNDEF);
    key_t delete_edge(key_t start_node_key, key_t edge_key);
    // const Node &
    // access_node(key_t node_key) const &; const Edge& access_edge(key_t
    // edge_key) const &;
    bool node_exists(key_t key) const;

  private:
    std::map<key_t, Node<N, E> *> m_nodes{};
    // матрица смежности (ключ - ключ вершины)
    std::map<key_t, std::list<Edge<N, E> *>> m_edges{};
};

template <typename N, typename E> key_t Graph<N, E>::add_node(N node_data, key_t node_key) {
    Node<N, E> *nptr = new Node<N, E>{node_data, node_key};
    if (nptr == nullptr) {
        return KEY_UNDEF;
    }
    auto key = nptr->get_key();
    if (key == KEY_UNDEF) {
        delete nptr;
        return KEY_UNDEF;
    }
    auto node_insertion_result = m_nodes.insert(std::make_pair(key, nptr));
    if (!node_insertion_result.second) {
        return KEY_UNDEF;
    }
    auto edge_init_result = m_edges.insert(std::make_pair(key, std::list<Edge<N, E> *>{}));
    if (!edge_init_result.second) {
        m_nodes.erase(key);
        return KEY_UNDEF;
    }
    return key;
}

template <typename N, typename E> key_t Graph<N, E>::delete_node(key_t node_key) {
    auto result = m_nodes.find(node_key);
    if (result == m_nodes.end()) {
        return KEY_UNDEF;
    }
    auto edge_list_result = m_edges.find(node_key);
    if (edge_list_result == m_edges.end()) {
        return KEY_UNDEF;
    }

    delete result->second;
    delete edge_list_result->second;
    return node_key;
}

template <typename N, typename E>
key_t Graph<N, E>::add_edge(E edge_data, key_t start_node_key, key_t end_node_key, key_t edge_key) {
    // check if nodes exist
    auto start_node_it = m_nodes.find(start_node_key);
    if (start_node_it == m_nodes.end()) {
        return KEY_UNDEF;
    }
    auto end_node_it = m_nodes.find(start_node_key);
    if (end_node_it == m_nodes.end()) {
        return KEY_UNDEF;
    }

    Edge<N, E> *eptr = new Edge<N, E>{edge_data, start_node_key, end_node_key, edge_key};
    if (eptr == nullptr) {
        return KEY_UNDEF;
    }
    // check if edge key is valid
    auto key = eptr->get_key();
    if (key == KEY_UNDEF) {
        delete eptr;
        return KEY_UNDEF;
    }

    auto edge_list_item = m_edges.find(start_node_key);
    edge_list_item->second.push_back(eptr);
    return edge_key;
}

template <typename N, typename E>
key_t Graph<N, E>::delete_edge(key_t start_node_key, key_t edge_key) {
    auto node_edges_item = m_edges.find(start_node_key);
    if (node_edges_item == m_edges.end()) {
        return KEY_UNDEF;
    }
    Edge<N, E> *eptr = nullptr;
    for (auto elem : node_edges_item.second) {
        if (elem->get_key() == edge_key) {
            delete elem;
            return edge_key;
        }
    }
    return KEY_UNDEF;
}

template <typename N, typename E> bool Graph<N, E>::node_exists(key_t key) const {
    if (m_nodes.find(key) == m_nodes.end()) {
        return false;
    }
    return true;
}

} // namespace G
