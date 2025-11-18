// Network.h - םמגûי פאיכ
#ifndef NETWORK_H
#define NETWORK_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
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

public:
    void addConnection(int pipeId, int startKS, int endKS, int diameter);
    bool isPipeUsed(int pipeId) const;
    bool hasConnection(int startKS, int endKS) const;
    std::vector<int> topologicalSort() const;
    void displayNetwork() const;
    void clear();
    const std::unordered_map<int, std::vector<Edge>>& getAdjacencyList() const;
};

#endif