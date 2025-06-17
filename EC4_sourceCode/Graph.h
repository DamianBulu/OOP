#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <algorithm>

template <typename T>
class Graph {
private:
    // Adjacency list representation using unordered_map for O(1) access
    std::unordered_map<T, std::vector<T>> adjacencyList;

public:
    void addNode(const T& node) {
        if (adjacencyList.find(node) == adjacencyList.end()) {
            adjacencyList[node] = std::vector<T>();
        }
    }

    void addEdge(const T& node1, const T& node2) {
        addNode(node1);
        addNode(node2);

        adjacencyList[node1].push_back(node2);
        adjacencyList[node2].push_back(node1);
    }

    std::vector<T> getNeighbors(const T& node) const {
        auto it = adjacencyList.find(node);
        if (it != adjacencyList.end()) {
            return it->second;
        }
        return std::vector<T>();
    }

    bool hasNode(const T& node) const {
        return adjacencyList.find(node) != adjacencyList.end();
    }

    std::vector<T> getAllNodes() const {
        std::vector<T> nodes;
        for (const auto& pair : adjacencyList) {
            nodes.push_back(pair.first);
        }
        return nodes;
    }

    std::vector<T> findShortestPath(const T& start, const T& target) const {
        if (!hasNode(start) || !hasNode(target)) {
            return std::vector<T>();
        }

        if (start == target) {
            return std::vector<T>{start};
        }

        std::queue<T> queue;
        std::unordered_set<T> visited;
        std::unordered_map<T, T> parent;

        queue.push(start);
        visited.insert(start);

        while (!queue.empty()) {
            T current = queue.front();
            queue.pop();

            if (current == target) {
                // Reconstruct path
                std::vector<T> path;
                T node = target;
                while (node != start) {
                    path.push_back(node);
                    node = parent[node];
                }
                path.push_back(start);
                std::reverse(path.begin(), path.end());
                return path;
            }

            for (const T& neighbor : getNeighbors(current)) {
                if (visited.find(neighbor) == visited.end()) {
                    visited.insert(neighbor);
                    parent[neighbor] = current;
                    queue.push(neighbor);
                }
            }
        }

        return std::vector<T>();
    }

    size_t size() const {
        return adjacencyList.size();
    }

    void clear() {
        adjacencyList.clear();
    }
};

#endif // GRAPH_H
