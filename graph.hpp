#include <unordered_set>

using key_t = long long;
using weight_t = int;

template<typename T>
class Node {
public:
	explicit Node(int key);
	Node(T data);
	
private:
	static key_t m_key{0};
	weight_t m_val{0};
	T m_data{};
};

template<typename T>
class Edge {
public:
	explicit Edge(int key{0});
	Edge(T data, int key{0});
private:
	static key_t m_key{0};
	weight_t m_val{0}
	T m_data{};
};

template<typename N, typename E>
class Graph {
public:
	Graph(const Graph&) = delete;
	Graph(const Graph&&) = delete;
	Graph& operator= (const Graph&) = delete;

	int addNode(N data);
	int addEdge(E data);
	const N& getNodeData(key_t k) const &;
	const E& getEdgeData(key_t k) const &;
private:
	std::unordered_set< std::list< Node<N>* > > m_nodes{};
};
