#include "Graph.h"
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <climits>
#include <stack>
#include <utility>
#include <set>
#include <sstream> // 新增：字符串流头文件
using namespace std;

// 新增：C++98 整数转字符串函数
string intToString(int num) {
    ostringstream oss;
    oss << num;
    return oss.str();
}

// 替代C++11 tuple的边结构体（C++98兼容）
template <typename V, typename W>
struct EdgeTuple {
    W weight;
    V u;
    V v;
    EdgeTuple(W w, V a, V b) : weight(w), u(a), v(b) {}
};

// Tarjan算法结果结构体
template <typename V, typename W>
struct TarjanResult {
    vector<vector<V> > biconnectedComponents; // 双连通分量（边集合）
    set<V> articulationPoints;                // 关节点（替换unordered_set）
};

// ================= 实验1：邻接矩阵输出 =================
template <typename V, typename W>
void printAdjacencyMatrix(const Graph<V, W>& graph) {
    vector<V> vertices = graph.getVertices();
    int n = vertices.size();
    map<V, int> idxMap; // 顶点→索引映射

    // 构建索引映射
    for (int i = 0; i < n; ++i) {
        idxMap.insert(pair<V, int>(vertices[i], i));
    }

    // 打印表头（顶点名）
    cout << "===== 邻接矩阵 =====" << endl;
    cout << "   ";
    for (typename vector<V>::const_iterator it = vertices.begin(); it != vertices.end(); ++it) {
        cout << *it << "  ";
    }
    cout << endl;

    // 打印矩阵内容
    for (int i = 0; i < n; ++i) {
        cout << vertices[i] << "  ";
        for (int j = 0; j < n; ++j) {
            try {
                W weight = graph.getEdgeWeight(vertices[i], vertices[j]);
                cout << weight << "  ";
            } catch (...) {
                cout << "0  "; // 无边则输出0
            }
        }
        cout << endl;
    }
    cout << endl;
}

// ================= 实验3：Kruskal最小生成树 =================
// 边排序比较函数（C++98函数对象）
template <typename V, typename W>
struct EdgeCompare {
    bool operator()(const EdgeTuple<V, W>& a, const EdgeTuple<V, W>& b) {
        return a.weight < b.weight;
    }
};

// 并查集find函数（递归，C++98无lambda）
template <typename T>
int findParent(T& parent, int x) {
    if (parent[x] != x) {
        parent[x] = findParent(parent, parent[x]); // 路径压缩
    }
    return parent[x];
}

// 并查集unite函数（合并两个集合）
template <typename T>
bool uniteParent(T& parent, int x, int y) {
    x = findParent(parent, x);
    y = findParent(parent, y);
    if (x != y) {
        parent[y] = x;
        return true;
    }
    return false;
}

// Kruskal算法实现
template <typename V, typename W>
Graph<V, W> kruskalMST(const Graph<V, W>& graph) {
    if (graph.isDirected()) {
        cerr << "Kruskal算法仅支持无向图！" << endl;
        return Graph<V, W>(false);
    }

    vector<V> vertices = graph.getVertices();
    int n = vertices.size();
    Graph<V, W> mst(false); // 最小生成树为无向图

    // 添加所有顶点到MST
    for (typename vector<V>::const_iterator it = vertices.begin(); it != vertices.end(); ++it) {
        mst.addVertex(*it);
    }

    // 步骤1：提取所有边（去重，无向图仅保留u<v的边）
    vector<EdgeTuple<V, W> > edges;
    map<V, int> idxMap;
    for (int i = 0; i < n; ++i) {
        idxMap.insert(pair<V, int>(vertices[i], i));
    }

    for (int i = 0; i < n; ++i) {
        V u = vertices[i];
        const vector<Edge<V, W> >& adj = graph.getAdjList(i);
        for (typename vector<Edge<V, W> >::const_iterator eIt = adj.begin(); eIt != adj.end(); ++eIt) {
            V v = vertices[eIt->to];
            if (idxMap[u] < idxMap[v]) { // 避免重复边
                edges.push_back(EdgeTuple<V, W>(eIt->weight, u, v));
            }
        }
    }

    // 步骤2：按权重升序排序边
    EdgeCompare<V, W> cmp;
    sort(edges.begin(), edges.end(), cmp);

    // 步骤3：并查集初始化
    vector<int> parent(n);
    for (int i = 0; i < n; ++i) parent[i] = i;

    // 步骤4：贪心选边构建MST
    int edgeCount = 0;
    for (typename vector<EdgeTuple<V, W> >::iterator it = edges.begin(); it != edges.end(); ++it) {
        int uIdx = idxMap[it->u];
        int vIdx = idxMap[it->v];
        if (uniteParent(parent, uIdx, vIdx)) { // 无环则添加边
            mst.addEdge(it->u, it->v, it->weight);
            edgeCount++;
            if (edgeCount == n - 1) break; // MST需n-1条边
        }
    }

    return mst;
}

// ================= 实验4：Tarjan双连通分量与关节点 =================
// Tarjan递归辅助函数（抽离避免lambda，C++98兼容）
template <typename V, typename W>
void tarjanDFS(int u, int& time, vector<int>& disc, vector<int>& low, vector<int>& parent, 
               vector<bool>& isArticulation, stack<pair<V, V> >& edgeStack, 
               const Graph<V, W>& graph, const vector<V>& vertices, TarjanResult<V, W>& result) {
    disc[u] = low[u] = ++time;
    int childCount = 0; // 根节点子节点数

    const vector<Edge<V, W> >& adj = graph.getAdjList(u);
    for (typename vector<Edge<V, W> >::const_iterator eIt = adj.begin(); eIt != adj.end(); ++eIt) {
        int v = eIt->to;
        if (disc[v] == -1) { // 未访问的子节点
            childCount++;
            parent[v] = u;
            edgeStack.push(pair<V, V>(vertices[u], vertices[v])); // 压入边

            // 递归处理子节点
            tarjanDFS(v, time, disc, low, parent, isArticulation, edgeStack, graph, vertices, result);
            low[u] = min(low[u], low[v]);

            // 情况1：根节点且子节点数≥2 → 关节点
            if (parent[u] == -1 && childCount > 1) {
                isArticulation[u] = true;
                // 弹出当前双连通分量的边
                vector<V> component;
                while (true) {
                    pair<V, V> edge = edgeStack.top();
                    edgeStack.pop();
                    component.push_back(edge.first);
                    component.push_back(edge.second);
                    if (edge.first == vertices[u] && edge.second == vertices[v]) break;
                }
                result.biconnectedComponents.push_back(component);
            }

            // 情况2：非根节点，low[v] ≥ disc[u] → 关节点
            if (parent[u] != -1 && low[v] >= disc[u]) {
                isArticulation[u] = true;
                // 弹出当前双连通分量的边
                vector<V> component;
                while (true) {
                    pair<V, V> edge = edgeStack.top();
                    edgeStack.pop();
                    component.push_back(edge.first);
                    component.push_back(edge.second);
                    if (edge.first == vertices[u] && edge.second == vertices[v]) break;
                }
                result.biconnectedComponents.push_back(component);
            }
        } 
        // 已访问的非父节点（回边）
        else if (v != parent[u] && disc[v] < disc[u]) {
            edgeStack.push(pair<V, V>(vertices[u], vertices[v]));
            low[u] = min(low[u], disc[v]);
        }
    }
}

// Tarjan算法主函数
template <typename V, typename W>
TarjanResult<V, W> tarjanBiconnected(const Graph<V, W>& graph) {
    TarjanResult<V, W> result;
    if (graph.isDirected()) {
        cerr << "Tarjan算法仅支持无向图！" << endl;
        return result;
    }

    vector<V> vertices = graph.getVertices();
    int n = vertices.size();
    map<V, int> idxMap;
    for (int i = 0; i < n; ++i) {
        idxMap.insert(pair<V, int>(vertices[i], i));
    }

    // 初始化Tarjan变量
    vector<int> disc(n, -1);    // 发现时间
    vector<int> low(n, -1);     // 最早可达发现时间
    vector<int> parent(n, -1);  // 父节点索引
    vector<bool> isArticulation(n, false); // 是否为关节点
    stack<pair<V, V> > edgeStack; // 存储双连通分量的边
    int time = 0;

    // 处理森林（多个连通分量）
    for (int i = 0; i < n; ++i) {
        if (disc[i] == -1) {
            tarjanDFS(i, time, disc, low, parent, isArticulation, edgeStack, graph, vertices, result);
            // 弹出剩余边（最后一个双连通分量）
            if (!edgeStack.empty()) {
                vector<V> component;
                while (!edgeStack.empty()) {
                    pair<V, V> edge = edgeStack.top();
                    edgeStack.pop();
                    component.push_back(edge.first);
                    component.push_back(edge.second);
                }
                result.biconnectedComponents.push_back(component);
            }
        }
    }

    // 收集关节点
    for (int i = 0; i < n; ++i) {
        if (isArticulation[i]) {
            result.articulationPoints.insert(vertices[i]);
        }
    }

    return result;
}

// 打印Tarjan算法结果
template <typename V, typename W>
void printTarjanResult(const TarjanResult<V, W>& res) {
    cout << "===== 双连通分量 =====" << endl;
    for (int i = 0; i < (int)res.biconnectedComponents.size(); ++i) {
        cout << "分量" << i + 1 << "：";
        const vector<V>& comp = res.biconnectedComponents[i];
        for (int j = 0; j < (int)comp.size(); j += 2) {
            cout << comp[j] << "-" << comp[j + 1];
            if (j < (int)comp.size() - 2) cout << ", ";
        }
        cout << endl;
    }

    cout << "===== 关节点 =====" << endl;
    for (typename set<V>::const_iterator it = res.articulationPoints.begin(); it != res.articulationPoints.end(); ++it) {
        cout << *it << " ";
    }
    cout << endl << endl;
}

// ================= 主函数：实验入口 =================
int main() {
    // ========== 实验1+2+3：图1（无向带权图） ==========
    cout << "===== 实验1+2+3：图1 操作 =====" << endl;
    // 1. 初始化顶点（C++98数组+循环）
    vector<char> vertices1;
    char v1_arr[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L'};
    int v1_size = sizeof(v1_arr) / sizeof(char);
    for (int i = 0; i < v1_size; ++i) {
        vertices1.push_back(v1_arr[i]);
    }

    // 2. 初始化边（替代C++11 tuple）
    vector<EdgeTuple<char, int> > edges1;
    edges1.push_back(EdgeTuple<char, int>(7, 'A', 'B'));
    edges1.push_back(EdgeTuple<char, int>(6, 'A', 'D'));
    edges1.push_back(EdgeTuple<char, int>(4, 'A', 'I'));
    edges1.push_back(EdgeTuple<char, int>(13, 'A', 'J'));
    edges1.push_back(EdgeTuple<char, int>(9, 'B', 'E'));
    edges1.push_back(EdgeTuple<char, int>(11, 'B', 'K'));
    edges1.push_back(EdgeTuple<char, int>(12, 'C', 'F'));
    edges1.push_back(EdgeTuple<char, int>(1, 'C', 'L'));
    edges1.push_back(EdgeTuple<char, int>(2, 'D', 'G'));
    edges1.push_back(EdgeTuple<char, int>(14, 'E', 'H'));
    edges1.push_back(EdgeTuple<char, int>(5, 'F', 'G'));
    edges1.push_back(EdgeTuple<char, int>(8, 'G', 'H'));
    edges1.push_back(EdgeTuple<char, int>(2, 'H', 'L'));
    edges1.push_back(EdgeTuple<char, int>(3, 'I', 'J'));
    edges1.push_back(EdgeTuple<char, int>(10, 'J', 'K'));
    edges1.push_back(EdgeTuple<char, int>(3, 'K', 'L'));

    // 3. 构建无向图
    Graph<char, int> graph1(false);
    for (int i = 0; i < v1_size; ++i) {
        graph1.addVertex(vertices1[i]);
    }
    for (int i = 0; i < (int)edges1.size(); ++i) {
        graph1.addEdge(edges1[i].u, edges1[i].v, edges1[i].weight);
    }

    // 4. 实验1：输出邻接矩阵
    printAdjacencyMatrix(graph1);

    // 5. 实验2：BFS/DFS遍历（从A出发）
    vector<char> bfsRes = graph1.bfs('A');
    cout << "===== BFS遍历（A起点） =====" << endl;
    for (int i = 0; i < (int)bfsRes.size(); ++i) {
        cout << bfsRes[i] << " ";
    }
    cout << endl << endl;

    vector<char> dfsRes = graph1.dfs('A');
    cout << "===== DFS遍历（A起点） =====" << endl;
    for (int i = 0; i < (int)dfsRes.size(); ++i) {
        cout << dfsRes[i] << " ";
    }
    cout << endl << endl;

    // 6. 实验3：Dijkstra最短路径（A起点）
    map<char, int> dist = graph1.dijkstra('A');
    cout << "===== Dijkstra最短路径（A起点） =====" << endl;
    for (int i = 0; i < v1_size; ++i) {
        char v = vertices1[i];
        map<char, int>::iterator it = dist.find(v);
        // 修复：统一三元运算符类型为const char*
        const char* distStr = (it->second == -1) ? "不可达" : intToString(it->second).c_str();
        cout << "A→" << v << "：" << distStr << endl;
    }
    cout << endl;

    // 7. 实验3：Kruskal最小生成树
    Graph<char, int> mst = kruskalMST(graph1);
    cout << "===== Kruskal最小生成树（邻接表） =====" << endl;
    mst.print();
    cout << endl;

    // ========== 实验4：图2（双连通分量+关节点） ==========
    cout << "===== 实验4：图2 双连通分量与关节点 =====" << endl;
    // 1. 初始化顶点（修复多字符常量：10→a,11→b,12→c,13→d）
    vector<char> vertices2;
    char v2_arr[] = {'1','2','3','4','5','6','7','8','9','a','b','c','d','H','D','E','G','B'};
    int v2_size = sizeof(v2_arr) / sizeof(char);
    for (int i = 0; i < v2_size; ++i) {
        vertices2.push_back(v2_arr[i]);
    }

    // 2. 初始化边（同步修改多字符顶点为单字符）
    vector<EdgeTuple<char, int> > edges2;
    edges2.push_back(EdgeTuple<char, int>(1, '1', '2'));
    edges2.push_back(EdgeTuple<char, int>(1, '1', '5'));
    edges2.push_back(EdgeTuple<char, int>(1, '2', '3'));
    edges2.push_back(EdgeTuple<char, int>(1, '3', 'H'));
    edges2.push_back(EdgeTuple<char, int>(1, '4', 'd')); 
    edges2.push_back(EdgeTuple<char, int>(1, '5', '8'));
    edges2.push_back(EdgeTuple<char, int>(1, '6', 'D'));
    edges2.push_back(EdgeTuple<char, int>(1, '6', '7'));
    edges2.push_back(EdgeTuple<char, int>(1, '7', 'G'));
    edges2.push_back(EdgeTuple<char, int>(1, '9', 'E'));
    edges2.push_back(EdgeTuple<char, int>(1, '9', 'B'));
    edges2.push_back(EdgeTuple<char, int>(1, 'a', 'b')); 
    edges2.push_back(EdgeTuple<char, int>(1, 'b', 'd')); 
    edges2.push_back(EdgeTuple<char, int>(1, 'c', '1')); 
    edges2.push_back(EdgeTuple<char, int>(1, 'E', 'e')); 
    edges2.push_back(EdgeTuple<char, int>(1, 'D', '2'));
    edges2.push_back(EdgeTuple<char, int>(1, 'G', '5'));
    edges2.push_back(EdgeTuple<char, int>(1, 'B', 'c')); 

    // 3. 构建无向图
    Graph<char, int> graph2(false);
    for (int i = 0; i < v2_size; ++i) {
        graph2.addVertex(vertices2[i]);
    }
    for (int i = 0; i < (int)edges2.size(); ++i) {
        graph2.addEdge(edges2[i].u, edges2[i].v, edges2[i].weight);
    }

    // 4. Tarjan算法求解
    TarjanResult<char, int> tarjanRes = tarjanBiconnected(graph2);
    printTarjanResult(tarjanRes);

    return 0;
}