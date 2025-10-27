#ifndef QUEUE_H
#define QUEUE_H
#include <iostream>
#include <cstdlib>
using namespace std;

template <typename T> class Queue {
private:
    // 嵌套节点结构
    struct Node {
        T data;      
        Node* next;  
        // 节点构造函数
        Node(T const& e, Node* n = nullptr) : data(e), next(n) {}
    };

    Node* frontNode;  
    Node* rearNode;   
    int _size;       

public:
    // 构造函数
    Queue() : frontNode(nullptr), rearNode(nullptr), _size(0) {}

    // 析构函数
    ~Queue() {
        clear();
    }

    // 拷贝构造函数
    Queue(Queue<T> const& q) {
        frontNode = nullptr;
        rearNode = nullptr;
        _size = 0;
        Node* p = q.frontNode;
        while (p) {
            enqueue(p->data);
            p = p->next;
        }
    }

    // 赋值运算符
    Queue<T>& operator=(Queue<T> const& q) {
        if (this != &q) {
            clear();  
            Node* p = q.frontNode;
            while (p) {
                enqueue(p->data);
                p = p->next;
            }
        }
        return *this;
    }

    // 基本操作
    int size() const { return _size; }        
    bool empty() const { return _size == 0; } 

    // 入队：在队尾插入元素
    void enqueue(T const& e) {
        Node* newNode = new Node(e);
        if (empty()) {
            frontNode = newNode;  
            rearNode = newNode;
        } else {
            rearNode->next = newNode; 
            rearNode = newNode;
        }
        _size++;
    }

    // 出队：删除并返回队首元素
    T dequeue() {
        if (empty()) {
            cerr << "队列为空，无法执行dequeue操作！" << endl;
            exit(EXIT_FAILURE);
        }
        Node* oldFront = frontNode;
        T e = oldFront->data;
        frontNode = frontNode->next; 
        delete oldFront;
        _size--;
        // 若队列变空，队尾指针也置空
        if (empty()) {
            rearNode = nullptr;
        }
        return e;
    }

    // 查看队首元素（不删除）
    T& front() const {
        if (empty()) {
            cerr << "队列为空，无法执行front操作！" << endl;
            exit(EXIT_FAILURE);
        }
        return frontNode->data;
    }

    // 查看队尾元素（不删除）
    T& back() const {
        if (empty()) {
            cerr << "队列为空，无法执行back操作！" << endl;
            exit(EXIT_FAILURE);
        }
        return rearNode->data;
    }

    // 清空队列
    void clear() {
        while (!empty()) {
            dequeue();
        }
    }

    // 遍历输出队列元素（从队首到队尾）
    void print() const {
        Node* p = frontNode;
        while (p) {
            cout << p->data << " ";
            p = p->next;
        }
        cout << endl;
    }
};



#endif  // QUEUE_H