#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <map>
#include <queue>
#include <climits>
#include <iostream>
#include <utility>
using namespace std;

// 边结构体（C++98无列表初始化，显式构造函数）
template <typename V, typename W>
struct Edge {
    int to;       // 邻接顶点索引
    W weight;     // 边权重
    Edge(int t, W w) : to(t), weight(w) {} // 显式构造函数
};

// 图类（无向/有向通用，C++98纯实现）
template <typename V, typename W>
class Graph {
private:
    vector<vector<Edge<V, W> > > adjList; // 邻接表（C++98空格分隔）
    map<V, int> vertexMap;                // 顶点→索引（替换unordered_map）
    vector<V> vertices;                   // 索引→顶点
    bool isDirectedGraph;                 // 是否有向图

public:
    // 构造函数（C++98无默认参数简化）
    Graph(bool directed) : isDirectedGraph(directed) {}

    // 添加顶点
    void addVertex(const V& v) {
        if (vertexMap.find(v) == vertexMap.end()) {
            vertexMap.insert(pair<V, int>(v, vertices.size())); // C++98 pair插入
            vertices.push_back(v);
            adjList.push_back(vector<Edge<V, W> >()); // 空邻接表初始化
        }
    }

    // 添加边（无向图自动双向添加）
    void addEdge(const V& u, const V& v, W weight) {
        typename map<V, int>::iterator uIt = vertexMap.find(u);
        typename map<V, int>::iterator vIt = vertexMap.find(v);
        if (uIt == vertexMap.end() || vIt == vertexMap.end()) {
            cerr << "Error: 顶点不存在！" << endl;
            return;
        }
        int uIdx = uIt->second;
        int vIdx = vIt->second;
        adjList[uIdx].push_back(Edge<V, W>(vIdx, weight));
        if (!isDirectedGraph) { // 无向图双向边
            adjList[vIdx].push_back(Edge<V, W>(uIdx, weight));
        }
    }

    // 获取边权重（不存在则抛异常）→ 关键：添加const修饰
    W getEdgeWeight(const V& u, const V& v) const {
        typename map<V, int>::const_iterator uIt = vertexMap.find(u); // const迭代器
        typename map<V, int>::const_iterator vIt = vertexMap.find(v);
        if (uIt == vertexMap.end() || vIt == vertexMap.end()) {
            throw "Vertex not found";
        }
        int uIdx = uIt->second;
        int vIdx = vIt->second;
        // 遍历邻接表用const_iterator
        for (typename vector<Edge<V, W> >::const_iterator it = adjList[uIdx].begin(); it != adjList[uIdx].end(); ++it) {
            if (it->to == vIdx) {
                return it->weight;
            }
        }
        throw "Edge not found";
    }

    // 获取顶点列表 → 加const（可选，规范写法）
    vector<V> getVertices() const {
        return vertices;
    }

    // 获取指定顶点的邻接表（对外提供）→ 加const
    const vector<Edge<V, W> >& getAdjList(int idx) const {
        return adjList[idx];
    }

    // 判断是否有向图 → 加const
    bool isDirected() const {
        return isDirectedGraph;
    }

    // BFS遍历（从指定顶点出发）
    vector<V> bfs(const V& start) {
        vector<V> result;
        typename map<V, int>::iterator it = vertexMap.find(start);
        if (it == vertexMap.end()) return result; // 起点不存在
        int startIdx = it->second;

        vector<bool> visited(vertices.size(), false);
        queue<int> q;
        q.push(startIdx);
        visited[startIdx] = true;

        while (!q.empty()) {
            int u = q.front();
            q.pop();
            result.push_back(vertices[u]);

            // 遍历邻接边
            for (typename vector<Edge<V, W> >::iterator eIt = adjList[u].begin(); eIt != adjList[u].end(); ++eIt) {
                int v = eIt->to;
                if (!visited[v]) {
                    visited[v] = true;
                    q.push(v);
                }
            }
        }
        return result;
    }

    // DFS辅助函数（递归）
    void dfsHelper(int u, vector<bool>& visited, vector<V>& result) {
        visited[u] = true;
        result.push_back(vertices[u]);
        for (typename vector<Edge<V, W> >::iterator eIt = adjList[u].begin(); eIt != adjList[u].end(); ++eIt) {
            int v = eIt->to;
            if (!visited[v]) {
                dfsHelper(v, visited, result);
            }
        }
    }

    // DFS遍历（从指定顶点出发）
    vector<V> dfs(const V& start) {
        vector<V> result;
        typename map<V, int>::iterator it = vertexMap.find(start);
        if (it == vertexMap.end()) return result;
        int startIdx = it->second;

        vector<bool> visited(vertices.size(), false);
        dfsHelper(startIdx, visited, result);
        return result;
    }

    // Dijkstra最短路径（单源，权重非负）
    map<V, int> dijkstra(const V& start) {
        map<V, int> dist; // 替换unordered_map
        typename map<V, int>::iterator it = vertexMap.find(start);
        if (it == vertexMap.end()) return dist;
        int startIdx = it->second;

        // 初始化距离数组（INT_MAX表示不可达）
        vector<int> distVec(vertices.size(), INT_MAX);
        distVec[startIdx] = 0;
        vector<bool> visited(vertices.size(), false);

        for (int i = 0; i < (int)vertices.size(); ++i) {
            // 找未访问的最小距离顶点
            int minDist = INT_MAX;
            int u = -1;
            for (int j = 0; j < (int)vertices.size(); ++j) {
                if (!visited[j] && distVec[j] < minDist) {
                    minDist = distVec[j];
                    u = j;
                }
            }
            if (u == -1) break; // 所有可达顶点已处理
            visited[u] = true;

            // 松弛操作
            for (typename vector<Edge<V, W> >::iterator eIt = adjList[u].begin(); eIt != adjList[u].end(); ++eIt) {
                int v = eIt->to;
                int weight = eIt->weight;
                if (!visited[v] && distVec[u] != INT_MAX && distVec[u] + weight < distVec[v]) {
                    distVec[v] = distVec[u] + weight;
                }
            }
        }

        // 转换为map返回（-1表示不可达）
        for (int i = 0; i < (int)vertices.size(); ++i) {
            if (distVec[i] == INT_MAX) {
                dist.insert(pair<V, int>(vertices[i], -1));
            } else {
                dist.insert(pair<V, int>(vertices[i], distVec[i]));
            }
        }
        return dist;
    }

    // 打印邻接表（调试用）
    void print() {
        for (int i = 0; i < (int)vertices.size(); ++i) {
            cout << vertices[i] << " → ";
            for (typename vector<Edge<V, W> >::iterator eIt = adjList[i].begin(); eIt != adjList[i].end(); ++eIt) {
                cout << vertices[eIt->to] << "(" << eIt->weight << ") ";
            }
            cout << endl;
        }
    }
};

#endif // GRAPH_H