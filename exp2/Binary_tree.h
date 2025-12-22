#ifndef BINARY_TREE_H
#define BINARY_TREE_H
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <functional>
using namespace std;

template <typename T> class BinaryTree {
private:
    struct Node {
        T data;
        int height;
        Node* left;
        Node* right;
        Node* parent;

        Node(T const& e, Node* l = NULL, Node* r = NULL, Node* p = NULL) 
            : data(e), left(l), right(r), parent(p), height(1) {}
    };

    Node* root;
    int _size;

    // 辅助：更新节点高度
    void updateHeight(Node* node) {
        if (!node) return;
        int leftH = node->left ? node->left->height : 0;
        int rightH = node->right ? node->right->height : 0;
        node->height = max(leftH, rightH) + 1;
    }

    // 辅助：递归销毁树
    void destroy(Node* node) {
        if (node) {
            destroy(node->left);
            destroy(node->right);
            delete node;
        }
    }

    // 辅助：递归拷贝树
    Node* copy(Node* node, Node* parent) {
        if (!node) return NULL;
        Node* newNode = new Node(node->data, NULL, NULL, parent);
        newNode->height = node->height;
        newNode->left = copy(node->left, newNode);
        newNode->right = copy(node->right, newNode);
        return newNode;
    }

    // 辅助：前/中/后序遍历
    void preOrder(Node* node) const {
        if (node) {
            cout << node->data << " ";
            preOrder(node->left);
            preOrder(node->right);
        }
    }
    void inOrder(Node* node) const {
        if (node) {
            inOrder(node->left);
            cout << node->data << " ";
            inOrder(node->right);
        }
    }
    void postOrder(Node* node) const {
        if (node) {
            postOrder(node->left);
            postOrder(node->right);
            cout << node->data << " ";
        }
    }

    // ===== 核心：递归判等（供operator==调用）=====
    bool equalTo(Node* a, Node* b) const {
        if (!a && !b) return true;  // 双空相等
        if (!a || !b) return false; // 一空一非空不等
        // 节点值相等 + 左子树相等 + 右子树相等
        return (a->data == b->data) 
               && equalTo(a->left, b->left) 
               && equalTo(a->right, b->right);
    }

    // ===== 核心：递归比较（供operator<调用）=====
    bool lessThan(Node* a, Node* b) const {
        // 空树优先级低于非空树
        if (!a && b) return true;   // a空b非空 → a < b
        if (a && !b) return false;  // a非空b空 → a > b
        if (!a && !b) return false; // 双空相等 → a 不小于 b

        // 1. 先比较当前节点值
        if (a->data < b->data) return true;
        if (a->data > b->data) return false;

        // 2. 当前节点值相等，比较左子树
        if (lessThan(a->left, b->left)) return true;
        if (lessThan(b->left, a->left)) return false;

        // 3. 左子树相等，比较右子树
        return lessThan(a->right, b->right);
    }

    // 扩展：带自定义规则的判等（备用）
    template <typename Equal>
    bool equalTo(Node* a, Node* b, Equal eq) const {
        if (!a && !b) return true;
        if (!a || !b) return false;
        return eq(a->data, b->data) 
               && equalTo(a->left, b->left, eq) 
               && equalTo(a->right, b->right, eq);
    }

    // 扩展：带自定义规则的比较（备用）
    template <typename Compare>
    bool lessThan(Node* a, Node* b, Compare comp) const {
        if (!a && b) return true;
        if (a && !b) return false;
        if (!a && !b) return false;

        if (comp(a->data, b->data)) return true;
        if (comp(b->data, a->data)) return false;

        if (lessThan(a->left, b->left, comp)) return true;
        if (lessThan(b->left, a->left, comp)) return false;

        return lessThan(a->right, b->right, comp);
    }

public:
    // 构造/析构/拷贝/赋值
    BinaryTree() : root(NULL), _size(0) {}
    ~BinaryTree() { clear(); }
    BinaryTree(BinaryTree<T> const& tree) {
        root = copy(tree.root, NULL);
        _size = tree._size;
    }
    BinaryTree<T>& operator=(BinaryTree<T> const& tree) {
        if (this != &tree) {
            clear();
            root = copy(tree.root, NULL);
            _size = tree._size;
        }
        return *this;
    }

    // 基础操作
    int size() const { return _size; }
    bool empty() const { return _size == 0; }
    void setRoot(T const& e) {
        if (root) delete root;
        root = new Node(e);
        _size = 1;
    }
    void addLeft(Node* parent, T const& e) {
        if (!parent) {
            cerr << "父节点为空，无法添加左子节点！" << endl;
            return;
        }
        if (parent->left) {
            destroy(parent->left);
            _size--;
        }
        parent->left = new Node(e, NULL, NULL, parent);
        _size++;
        Node* curr = parent;
        while (curr) {
            updateHeight(curr);
            curr = curr->parent;
        }
    }
    void addRight(Node* parent, T const& e) {
        if (!parent) {
            cerr << "父节点为空，无法添加右子节点！" << endl;
            return;
        }
        if (parent->right) {
            destroy(parent->right);
            _size--;
        }
        parent->right = new Node(e, NULL, NULL, parent);
        _size++;
        Node* curr = parent;
        while (curr) {
            updateHeight(curr);
            curr = curr->parent;
        }
    }

    // 节点访问
    Node* getRoot() const { return root; }
    Node* getLeft(Node* node) const { return node ? node->left : NULL; }
    Node* getRight(Node* node) const { return node ? node->right : NULL; }
    T& getData(Node* node) const {
        if (!node) {
            cerr << "节点为空，无法获取数据！" << endl;
            exit(EXIT_FAILURE);
        }
        return node->data;
    }
    int getHeight(Node* node) const { return node ? node->height : 0; }

    // 遍历
    void preOrder() const {
        cout << "前序遍历: ";
        preOrder(root);
        cout << endl;
    }
    void inOrder() const {
        cout << "中序遍历: ";
        inOrder(root);
        cout << endl;
    }
    void postOrder() const {
        cout << "后序遍历: ";
        postOrder(root);
        cout << endl;
    }
    void clear() {
        destroy(root);
        root = NULL;
        _size = 0;
    }

    // 判等器
    bool operator==(const BinaryTree<T>& other) const {
        // 先比较节点数量，数量不等直接返回false（优化）
        if (this->_size != other._size) return false;
        return equalTo(this->root, other.root);
    }

    // 判等器补充
    bool operator!=(const BinaryTree<T>& other) const {
        return !(*this == other);
    }

    // 比较器
    bool operator<(const BinaryTree<T>& other) const {
        return lessThan(this->root, other.root);
    }

    // 重载其他比较运算符
    bool operator>(const BinaryTree<T>& other) const {
        return other < *this;
    }
    bool operator<=(const BinaryTree<T>& other) const {
        return !(*this > other);
    }
    bool operator>=(const BinaryTree<T>& other) const {
        return !(*this < other);
    }

    // 自定义判等（例如：忽略整数绝对值、字符串大小写）
    template <typename Equal>
    bool equals(const BinaryTree<T>& other, Equal eq) const {
        if (this->_size != other._size) return false;
        return equalTo(this->root, other.root, eq);
    }

    // 自定义比较（例如：降序、按节点高度比较）
    template <typename Compare>
    bool isLessThan(const BinaryTree<T>& other, Compare comp) const {
        return lessThan(this->root, other.root, comp);
    }
};

#endif  // BINARY_TREE_H