#include "Network.h"
#include <iostream>
#include <algorithm>
#include <queue>
#include <set>
#include <limits>
#include <cmath>

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

vector<int> GasNetwork::getAllKS() const {
    set<int> ksSet;

    for (const auto& pair : adjacencyList) {
        ksSet.insert(pair.first);
        for (const auto& edge : pair.second) {
            ksSet.insert(edge.endKS);
        }
    }

    // Также добавляем вершины, которые только имеют входящие ребра
    for (const auto& pair : inDegree) {
        ksSet.insert(pair.first);
    }

    return vector<int>(ksSet.begin(), ksSet.end());
}

vector<pair<int, Edge>> GasNetwork::getNeighbors(int ks) const {
    vector<pair<int, Edge>> neighbors;

    auto it = adjacencyList.find(ks);
    if (it != adjacencyList.end()) {
        for (const auto& edge : it->second) {
            neighbors.emplace_back(edge.endKS, edge);
        }
    }

    return neighbors;
}

bool GasNetwork::bfsForFlow(int source, int sink,
    unordered_map<int, int>& parent,
    const unordered_map<int, unordered_map<int, double>>& capacity) const {
    queue<int> q;
    unordered_set<int> visited;

    q.push(source);
    visited.insert(source);
    parent[source] = -1;

    while (!q.empty()) {
        int current = q.front();
        q.pop();

        // Получаем соседей текущей вершины
        auto neighbors = getNeighbors(current);

        for (const auto& neighbor : neighbors) {
            int next = neighbor.first;

            // Проверяем, есть ли остаточная пропускная способность
            auto it_current = capacity.find(current);
            if (it_current != capacity.end()) {
                auto it_next = it_current->second.find(next);
                if (it_next != it_current->second.end() &&
                    it_next->second > 0.0001 && // Используем эпсилон для сравнения с нулем
                    visited.find(next) == visited.end()) {

                    q.push(next);
                    parent[next] = current;
                    visited.insert(next);

                    if (next == sink) {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

double GasNetwork::calculateMaxFlow(int sourceKS, int sinkKS,
    const unordered_map<int, Pipe>& pipes) const {
    if (adjacencyList.empty()) {
        return 0.0;
    }

    // Создаем карту пропускных способностей
    unordered_map<int, unordered_map<int, double>> capacity;

    // Заполняем пропускные способности из труб
    for (const auto& pair : adjacencyList) {
        int from = pair.first;

        for (const auto& edge : pair.second) {
            int to = edge.endKS;

            // Получаем производительность трубы
            auto pipeIt = pipes.find(edge.pipeId);
            if (pipeIt != pipes.end()) {
                double pipe_capacity = pipeIt->second.calculateCapacity();
                capacity[from][to] = pipe_capacity;
            }
            else {
                // Если труба не найдена, устанавливаем нулевую производительность
                capacity[from][to] = 0.0;
            }
        }
    }

    // Создаем остаточную сеть (изначально равна исходной)
    unordered_map<int, unordered_map<int, double>> residual = capacity;

    // Добавляем обратные ребра с нулевой пропускной способностью
    for (const auto& pair : capacity) {
        int from = pair.first;
        for (const auto& edge : pair.second) {
            int to = edge.first;
            if (residual[to].find(from) == residual[to].end()) {
                residual[to][from] = 0.0;
            }
        }
    }

    double max_flow = 0.0;

    // Алгоритм Эдмондса-Карпа
    while (true) {
        unordered_map<int, int> parent;

        if (!bfsForFlow(sourceKS, sinkKS, parent, residual)) {
            break; // Увеличивающий путь не найден
        }

        // Находим минимальную пропускную способность на найденном пути
        double path_flow = numeric_limits<double>::max();

        for (int v = sinkKS; v != sourceKS; v = parent[v]) {
            int u = parent[v];
            path_flow = min(path_flow, residual[u][v]);
        }

        // Обновляем остаточные пропускные способности
        for (int v = sinkKS; v != sourceKS; v = parent[v]) {
            int u = parent[v];
            residual[u][v] -= path_flow;
            residual[v][u] += path_flow; // Для обратных ребер
        }

        max_flow += path_flow;
    }

    return max_flow;
}

pair<double, vector<int>> GasNetwork::findShortestPath(int startKS, int endKS,
    const unordered_map<int, Pipe>& pipes) const {
    // Алгоритм Дейкстры
    vector<int> allKS = getAllKS();

    if (allKS.empty() ||
        find(allKS.begin(), allKS.end(), startKS) == allKS.end() ||
        find(allKS.begin(), allKS.end(), endKS) == allKS.end()) {
        return { numeric_limits<double>::infinity(), {} };
    }

    unordered_map<int, double> dist;
    unordered_map<int, int> prev;

    // Используем set как приоритетную очередь (расстояние, вершина)
    set<pair<double, int>> pq;

    // Инициализация
    for (int ks : allKS) {
        dist[ks] = numeric_limits<double>::infinity();
        prev[ks] = -1;
    }

    dist[startKS] = 0.0;
    pq.insert({ 0.0, startKS });

    while (!pq.empty()) {
        auto current = *pq.begin();
        pq.erase(pq.begin());

        double current_dist = current.first;
        int u = current.second;

        // Если нашли более короткий путь через другую вершину, пропускаем
        if (current_dist > dist[u] + 0.0001) {
            continue;
        }

        // Получаем соседей
        auto neighbors = getNeighbors(u);

        for (const auto& neighbor : neighbors) {
            int v = neighbor.first;
            const Edge& edge = neighbor.second;

            // Получаем вес ребра (длину трубы)
            auto pipeIt = pipes.find(edge.pipeId);
            if (pipeIt == pipes.end()) {
                continue; // Труба не найдена
            }

            double weight = pipeIt->second.calculateWeight();

            // Проверяем, не в ремонте ли труба
            if (weight == numeric_limits<double>::infinity()) {
                continue; // Пропускаем трубы в ремонте
            }

            double alt = dist[u] + weight;

            if (alt < dist[v] - 0.0001) { // Используем эпсилон для сравнения
                // Удаляем старую пару из очереди
                auto it = pq.find({ dist[v], v });
                if (it != pq.end()) {
                    pq.erase(it);
                }

                // Обновляем расстояние и предыдущую вершину
                dist[v] = alt;
                prev[v] = u;
                pq.insert({ alt, v });
            }
        }
    }

    // Восстанавливаем путь
    vector<int> path;
    if (dist[endKS] < numeric_limits<double>::infinity()) {
        for (int at = endKS; at != -1; at = prev[at]) {
            path.push_back(at);
        }
        reverse(path.begin(), path.end());
    }

    return { dist[endKS], path };
}