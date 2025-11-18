// Network.cpp - новый файл
#include "Network.h"
#include <iostream>
#include <algorithm>

using namespace std;

void GasNetwork::addConnection(int pipeId, int startKS, int endKS, int diameter) {
    Edge edge{ pipeId, startKS, endKS, diameter };
    adjacencyList[startKS].push_back(edge);
    usedPipes.insert(pipeId);

    // Обновляем полустепени захода
    inDegree[endKS]++;
    if (inDegree.find(startKS) == inDegree.end()) {
        inDegree[startKS] = 0;
    }
}

bool GasNetwork::isPipeUsed(int pipeId) const {
    return usedPipes.find(pipeId) != usedPipes.end();
}

bool GasNetwork::hasConnection(int startKS, int endKS) const {
    auto it = adjacencyList.find(startKS);
    if (it != adjacencyList.end()) {
        for (const auto& edge : it->second) {
            if (edge.endKS == endKS) {
                return true;
            }
        }
    }
    return false;
}

vector<int> GasNetwork::topologicalSort() const {
    vector<int> result;
    unordered_map<int, int> tempInDegree = inDegree;
    queue<int> q;

    // Находим вершины с нулевой полустепенью захода
    for (const auto& pair : tempInDegree) {
        if (pair.second == 0) {
            q.push(pair.first);
        }
    }

    while (!q.empty()) {
        int current = q.front();
        q.pop();
        result.push_back(current);

        if (adjacencyList.find(current) != adjacencyList.end()) {
            for (const auto& edge : adjacencyList.at(current)) {
                tempInDegree[edge.endKS]--;
                if (tempInDegree[edge.endKS] == 0) {
                    q.push(edge.endKS);
                }
            }
        }
    }

    // Проверяем наличие циклов
    if (result.size() != tempInDegree.size()) {
        return {}; // Граф содержит циклы
    }

    return result;
}

void GasNetwork::displayNetwork() const {
    if (adjacencyList.empty()) {
        cout << "Gas network is empty.\n";
        return;
    }

    cout << "Gas Transport Network:\n";
    for (const auto& pair : adjacencyList) {
        cout << "KS " << pair.first << " -> ";
        for (const auto& edge : pair.second) {
            cout << "KS " << edge.endKS << " (Pipe " << edge.pipeId << ", Diameter: " << edge.diameter << "mm) ";
        }
        cout << "\n";
    }
}

void GasNetwork::clear() {
    adjacencyList.clear();
    inDegree.clear();
    usedPipes.clear();
}

const unordered_map<int, vector<Edge>>& GasNetwork::getAdjacencyList() const {
    return adjacencyList;
}