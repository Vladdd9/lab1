#ifndef NETWORK_H
#define NETWORK_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <set>
#include <limits>
#include <algorithm>
#include "Pipe.h"
#include "KS.h"

struct Edge {
    int pipeId;
    int startKS;
    int endKS;
    int diameter;
};

class GasNetwork {
private:
    std::unordered_map<int, std::vector<Edge>> adjacencyList;
    std::unordered_map<int, int> inDegree;
    std::unordered_set<int> usedPipes;

    // Вспомогательные методы
    bool bfsForFlow(int source, int sink,
        std::unordered_map<int, int>& parent,
        const std::unordered_map<int, std::unordered_map<int, double>>& capacity) const;

    std::vector<std::pair<int, Edge>> getNeighbors(int ks) const;
    std::vector<int> getAllKS() const;

public:
    void addConnection(int pipeId, int startKS, int endKS, int diameter);
    bool isPipeUsed(int pipeId) const;
    bool hasConnection(int startKS, int endKS) const;
    std::vector<int> topologicalSort() const;
    void displayNetwork() const;
    void clear();
    const std::unordered_map<int, std::vector<Edge>>& getAdjacencyList() const;

    // Новые методы для максимального потока и кратчайшего пути
    double calculateMaxFlow(int sourceKS, int sinkKS,
        const std::unordered_map<int, Pipe>& pipes) const;

    std::pair<double, std::vector<int>> findShortestPath(int startKS, int endKS,
        const std::unordered_map<int, Pipe>& pipes) const;
};

#endif