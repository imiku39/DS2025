#ifndef STACK_H
#define STACK_H
#include <iostream>
#include <cstdlib>
#include <cstddef>
using namespace std;

template <typename T> class Stack {
private:
    struct Node {
        T data;     
        Node* next;   

        Node(T const& e, Node* n = NULL) : data(e), next(n) {}
    };

    Node* top; 
    int _size;  

public:
    // 构造函数
    Stack() : top(NULL), _size(0) {}

    // 析构函数
    ~Stack() {
        while (!empty()) {
            pop();
        }
    }

    // 拷贝构造函数
    Stack(Stack<T> const& s) {
        top = NULL;
        _size = 0;
        Node* p = s.top;

        Stack<T> temp;
        while (p) {
            temp.push(p->data);
            p = p->next;
        }

        while (!temp.empty()) {
            push(temp.pop());
        }
    }

    // 赋值运算符
    Stack<T>& operator=(Stack<T> const& s) {
        if (this != &s) {
            // 清空当前栈
            while (!empty()) {
                pop();
            }
            // 拷贝元素
            Node* p = s.top;
            Stack<T> temp;
            while (p) {
                temp.push(p->data);
                p = p->next;
            }
            while (!temp.empty()) {
                push(temp.pop());
            }
        }
        return *this;
    }

    int size() const { return _size; }        
    bool empty() const { return _size == 0; } 

    // 入栈：在栈顶插入元素
    void push(T const& e) {
        top = new Node(e, top);
        _size++;
    }

    // 出栈：删除并返回栈顶元素
    T pop() {
        if (empty()) {
            cerr << "栈为空，无法执行pop操作！" << endl;
            exit(EXIT_FAILURE);
        }
        Node* oldTop = top;
        T e = oldTop->data;
        top = top->next;
        delete oldTop;
        _size--;
        return e;
    }

    // 取栈顶元素（不删除）
    T& peek() const {
        if (empty()) {
            cerr << "栈为空，无法执行peek操作！" << endl;
            exit(EXIT_FAILURE);
        }
        return top->data;
    }

    // 遍历输出栈元素（从栈顶到栈底）
    void print() const {
        Node* p = top;
        while (p) {
            cout << p->data << " ";
            p = p->next;
        }
        cout << endl;
    }

    // 清空栈
    void clear() {
        while (!empty()) {
            pop();
        }
    }
};



#endif  // STACK_H
