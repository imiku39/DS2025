#ifndef TREE_H
#define TREE_H
#include <iostream>
using namespace std;

template <typename T> class Tree {
private:
    // 二叉树节点结构
    struct Node {
        T data;       
        Node* left;     
        Node* right;  
        Node* parent;   

        // 节点构造函数
        Node(T const& e, Node* p = nullptr, Node* l = nullptr, Node* r = nullptr)
            : data(e), parent(p), left(l), right(r) {}
    };

    Node* root; 
    int _size; 

    // 辅助函数：递归销毁节点
    void destroy(Node* node) {
        if (node) {
            destroy(node->left);  
            destroy(node->right);  
            delete node;         
        }
    }

    // 辅助函数：递归拷贝树
    Node* copy(Node* orig, Node* parent) {
        if (!orig) return nullptr;
        Node* newNode = new Node(orig->data, parent);
        newNode->left = copy(orig->left, newNode);   
        newNode->right = copy(orig->right, newNode);
        return newNode;
    }

    // 递归先序遍历
    void preOrder(Node* node, void (*visit)(T&)) const {
        if (node) {
            visit(node->data);
            preOrder(node->left, visit);
            preOrder(node->right, visit);
        }
    }

    // 递归中序遍历
    void inOrder(Node* node, void (*visit)(T&)) const {
        if (node) {
            inOrder(node->left, visit);
            visit(node->data);
            inOrder(node->right, visit);
        }
    }

    // 递归后序遍历
    void postOrder(Node* node, void (*visit)(T&)) const {
        if (node) {
            postOrder(node->left, visit);
            postOrder(node->right, visit);
            visit(node->data);
        }
    }

    // 非递归先序遍历（手工模拟栈）
    void preOrderIter(Node* node, void (*visit)(T&)) const {
        if (!node) return;
        // 手工实现栈（固定大小，实际可动态扩容，此处简化）
        Node* stack[100];
        int top = -1;
        stack[++top] = node; 

        while (top != -1) {   
            Node* curr = stack[top--];  
            visit(curr->data);         

            // 右孩子先入栈
            if (curr->right) stack[++top] = curr->right;
            if (curr->left) stack[++top] = curr->left;
        }
    }

    // 非递归中序遍历（手工模拟栈）
    void inOrderIter(Node* node, void (*visit)(T&)) const {
        if (!node) return;
        Node* stack[100];
        int top = -1;
        Node* curr = node;

        while (top != -1 || curr) {
            // 左子树全入栈
            while (curr) {
                stack[++top] = curr;
                curr = curr->left;
            }
            // 出栈并访问
            curr = stack[top--];
            visit(curr->data);

            curr = curr->right;
        }
    }

    // 非递归后序遍历（手工模拟栈，记录访问标记）
    void postOrderIter(Node* node, void (*visit)(T&)) const {
        if (!node) return;
        Node* stack[100];
        bool visited[100] = {false}; 
        int top = -1;
        stack[++top] = node;

        while (top != -1) {
            Node* curr = stack[top];
            if (visited[top]) {     
                top--;
                visit(curr->data);  
            } else {
                visited[top] = true; 
                // 右、左子树入栈（保证左先于右处理）
                if (curr->right) {
                    stack[++top] = curr->right;
                    visited[top] = false;
                }
                if (curr->left) {
                    stack[++top] = curr->left;
                    visited[top] = false;
                }
            }
        }
    }

    // 层序遍历（手工模拟队列）
    void levelOrderIter(Node* node, void (*visit)(T&)) const {
        if (!node) return;
        // 手工实现队列（固定大小，循环队列逻辑）
        Node* queue[100];
        int front = 0, rear = 0;
        queue[rear++] = node; 

        while (front != rear) { 
            Node* curr = queue[front++]; 
            visit(curr->data);  

            if (curr->left) queue[rear++] = curr->left;
            if (curr->right) queue[rear++] = curr->right;
        }
    }

public:
    // 构造函数
    Tree() : root(nullptr), _size(0) {}
    Tree(T const& rootData) : root(new Node(rootData)), _size(1) {}

    // 拷贝构造函数
    Tree(Tree<T> const& tree) {
        root = copy(tree.root, nullptr);
        _size = tree._size;
    }

    // 析构函数
    ~Tree() { clear(); }

    // 赋值运算符
    Tree<T>& operator=(Tree<T> const& tree) {
        if (this != &tree) {
            clear();
            root = copy(tree.root, nullptr);
            _size = tree._size;
        }
        return *this;
    }

    // 基本操作
    int size() const { return _size; }
    bool empty() const { return _size == 0; }

    // 获取根节点数据
    T& rootData() const {
        if (empty()) {
            cerr << "树为空，无法访问根节点！" << endl;
            exit(EXIT_FAILURE);
        }
        return root->data;
    }

    // 插入左孩子
    Node* insertLeft(Node* parent, T const& e) {
        if (!parent || parent->left) {
            cerr << "插入左孩子失败！" << endl;
            return nullptr;
        }
        parent->left = new Node(e, parent);
        _size++;
        return parent->left;
    }

    // 插入右孩子
    Node* insertRight(Node* parent, T const& e) {
        if (!parent || parent->right) {
            cerr << "插入右孩子失败！" << endl;
            return nullptr;
        }
        parent->right = new Node(e, parent);
        _size++;
        return parent->right;
    }

    // 清空树
    void clear() {
        destroy(root);
        root = nullptr;
        _size = 0;
    }

    // 遍历操作（递归版）
    void preOrder(void (*visit)(T&)) const { preOrder(root, visit); }
    void inOrder(void (*visit)(T&)) const { inOrder(root, visit); }
    void postOrder(void (*visit)(T&)) const { postOrder(root, visit); }

    // 遍历操作（非递归版，无库依赖）
    void preOrderIter(void (*visit)(T&)) const { preOrderIter(root, visit); }
    void inOrderIter(void (*visit)(T&)) const { inOrderIter(root, visit); }
    void postOrderIter(void (*visit)(T&)) const { postOrderIter(root, visit); }
    void levelOrder(void (*visit)(T&)) const { levelOrderIter(root, visit); }

    // 打印树（层序遍历）
    void print() const {
        cout << "层序遍历: ";
        levelOrder([](T& e) { cout << e << " "; });
        cout << endl;
    }

    // 获取根节点（供测试用）
    Node* getRoot() const { return root; }
};

// 遍历回调函数：打印节点数据
template <typename T> void printNode(T& e) { cout << e << " "; }



#endif  // TREE_H