#ifndef VECTOR_H
#define VECTOR_H
#include <iostream>
#include <cstdlib>
#include <algorithm>
using namespace std;

typedef int Rank; // 秩
#define DEFAULT_CAPACITY 3 // 默认初始容量

template <typename T> class Vector {
private:
    Rank _size; // 规模
    int _capacity; // 容量
    T* _elem; // 数据区

    void copyFrom(T const* A, Rank lo, Rank hi); // 复制数组区间[A[lo], A[hi])
    void expand(); // 扩容
    void shrink(); // 缩容
    bool bubble(Rank lo, Rank hi); // 冒泡一趟
    void bubbleSort(Rank lo, Rank hi); // 冒泡排序
    Rank max(Rank lo, Rank hi); // 选取最大值
    void selectionSort(Rank lo, Rank hi); // 选择排序
    void merge(Rank lo, Rank mi, Rank hi); // 归并
    void mergeSort(Rank lo, Rank hi); // 归并排序
    Rank partition(Rank lo, Rank hi); // 快速排序划分
    void quickSort(Rank lo, Rank hi); // 快速排序
    void heapSort(Rank lo, Rank hi); // 堆排序

public:
    // 构造函数
    Vector(int c = DEFAULT_CAPACITY, int s = 0, T v = T()) { // 容量为c、规模为s、所有元素初始为v
        _elem = new T[_capacity = c];
        for (_size = 0; _size < s; _elem[_size++] = v);
    }
    Vector(T const* A, Rank n) { copyFrom(A, 0, n); } // 从数组A复制n个元素
    Vector(T const* A, Rank lo, Rank hi) { copyFrom(A, lo, hi); } // 复制数组区间[A[lo], A[hi])
    Vector(Vector<T> const& V) { copyFrom(V._elem, 0, V._size); } // 拷贝构造
    Vector(Vector<T> const& V, Rank lo, Rank hi) { copyFrom(V._elem, lo, hi); } // 复制区间[lo, hi)

    // 析构函数
    ~Vector() { delete[] _elem; }

    // 只读访问接口
    Rank size() const { return _size; } // 规模
    bool empty() const { return _size == 0; } // 是否为空
    int disordered() const; // 判断是否有序
    Rank find(T const& e) const { return find(e, 0, _size); } // 无序查找
    Rank find(T const& e, Rank lo, Rank hi) const; // 无序查找区间[lo, hi)
    Rank search(T const& e) const { return search(e, 0, _size); } // 有序查找
    Rank search(T const& e, Rank lo, Rank hi) const; // 有序查找区间[lo, hi)

    // 可写访问接口
    T& operator[](Rank r) const; // 重载下标运算符
    Vector<T>& operator=(Vector<T> const& V); // 重载赋值运算符
    T remove(Rank r); // 删除秩为r的元素
    int remove(Rank lo, Rank hi); // 删除区间[lo, hi)的元素
    Rank insert(Rank r, T const& e); // 插入元素
    Rank insert(T const& e) { return insert(_size, e); } // 默认在末尾插入
    void sort(Rank lo, Rank hi); // 区间排序
    void sort() { sort(0, _size); } // 整体排序
    void unsort(Rank lo, Rank hi); // 区间置乱
    void unsort() { unsort(0, _size); } // 整体置乱
    int deduplicate(); // 无序去重
    int uniquify(); // 有序去重

    // 遍历
    void traverse(void (*)(T&));
    template <typename VST> void traverse(VST&);

    // 输出接口
    void print() const {
        for (Rank i = 0; i < _size; ++i) {
            cout << _elem[i] << " ";
        }
        cout << endl;
    }
};

template <typename T> void Vector<T>::copyFrom(T const* A, Rank lo, Rank hi) {
    _elem = new T[_capacity = 2 * (hi - lo)];
    _size = 0;
    while (lo < hi) {
        _elem[_size++] = A[lo++];
    }
}

template <typename T> Vector<T>& Vector<T>::operator=(Vector<T> const& V) {
    if (_elem) delete[] _elem;
    copyFrom(V._elem, 0, V._size);
    return *this;
}

template <typename T> void Vector<T>::expand() {
    if (_size < _capacity) return;
    T* oldElem = _elem;
    _capacity = max(_capacity, DEFAULT_CAPACITY);
    _elem = new T[_capacity <<= 1];
    for (Rank i = 0; i < _size; ++i) {
        _elem[i] = oldElem[i];
    }
    delete[] oldElem;
}

template <typename T> void Vector<T>::shrink() {
    if (_capacity < DEFAULT_CAPACITY << 1) return;
    if (_size << 2 > _capacity) return;
    T* oldElem = _elem;
    _capacity >>= 1;
    _elem = new T[_capacity];
    for (Rank i = 0; i < _size; ++i) {
        _elem[i] = oldElem[i];
    }
    delete[] oldElem;
}

template <typename T> T& Vector<T>::operator[](Rank r) const {
    return _elem[r]; 
}

template <typename T> Rank Vector<T>::find(T const& e, Rank lo, Rank hi) const {
    while ((lo < hi--) && (e != _elem[hi])); 
    return hi; 
}

template <typename T> Rank Vector<T>::insert(Rank r, T const& e) {
    expand();
    for (Rank i = _size; i > r; --i) {
        _elem[i] = _elem[i - 1];
    }
    _elem[r] = e;
    _size++; 
    return r; 
}

template <typename T> T Vector<T>::remove(Rank r) {
    T e = _elem[r];
    remove(r, r + 1); 
    return e; 
}

template <typename T> int Vector<T>::remove(Rank lo, Rank hi) {
    if (lo == hi) return 0;
    while (hi < _size) {
        _elem[lo++] = _elem[hi++]; 
    }
    _size = lo;
    shrink();
    return hi - lo; 
}

template <typename T> int Vector<T>::deduplicate() {
    int oldSize = _size;
    Rank i = 1;
    while (i < _size) {
        if (find(_elem[i], 0, i) < 0) {
            i++;
        } else {
            remove(i); 
        }
    }
    return oldSize - _size; 
}

template <typename T> void Vector<T>::traverse(void (*visit)(T&)) {
    for (Rank i = 0; i < _size; ++i) {
        visit(_elem[i]);
    }
}

template <typename T> template <typename VST>
void Vector<T>::traverse(VST& visit) {
    for (Rank i = 0; i < _size; ++i) {
        visit(_elem[i]);
    }
}

template <typename T> int Vector<T>::disordered() const {
    int n = 0;
    for (Rank i = 1; i < _size; ++i) {
        if (_elem[i - 1] > _elem[i]) n++;
    }
    return n;
}

template <typename T> int Vector<T>::uniquify() {
    Rank i = 0, j = 0;
    while (++j < _size) { 
        if (_elem[i] != _elem[j]) {
            _elem[++i] = _elem[j];
        }
    }
    _size = ++i;
    shrink(); 
    return j - i;
}

template <typename T> Rank Vector<T>::search(T const& e, Rank lo, Rank hi) const {
    // 二分查找
    while (lo < hi) {
        Rank mi = (lo + hi) >> 1;
        if (e < _elem[mi]) {
            hi = mi;
        } else {
            lo = mi + 1;
        }
    }
    return --lo; 
}

template <typename T> void Vector<T>::sort(Rank lo, Rank hi) {
    switch (rand() % 5) { // 随机选择排序算法
        case 0: bubbleSort(lo, hi); break;
        case 1: selectionSort(lo, hi); break;
        case 2: mergeSort(lo, hi); break;
        case 3: quickSort(lo, hi); break;
        case 4: heapSort(lo, hi); break;
    }
}

template <typename T> bool Vector<T>::bubble(Rank lo, Rank hi) {
    bool sorted = true;
    while (++lo < hi) { 
        if (_elem[lo - 1] > _elem[lo]) {
            swap(_elem[lo - 1], _elem[lo]);
            sorted = false;
        }
    }
    return sorted;
}

template <typename T> void Vector<T>::bubbleSort(Rank lo, Rank hi) {
    while (!bubble(lo, hi--)); // 冒泡
}

template <typename T> Rank Vector<T>::max(Rank lo, Rank hi) {
    Rank maxIdx = hi - 1;
    while (lo < --hi) {
        if (_elem[hi] > _elem[maxIdx]) {
            maxIdx = hi;
        }
    }
    return maxIdx;
}

template <typename T> void Vector<T>::selectionSort(Rank lo, Rank hi) {
    while (lo < --hi) {
        swap(_elem[max(lo, hi)], _elem[hi]);
    }
}

template <typename T> void Vector<T>::merge(Rank lo, Rank mi, Rank hi) {
    T* A = _elem + lo; 
    int len = mi - lo;
    T* B = new T[len]; 
    for (Rank i = 0; i < len; B[i] = A[i++]); 
    Rank i = 0, j = mi, k = lo;
    while (i < len && j < hi) { 
        A[k++] = (B[i] <= _elem[j]) ? B[i++] : _elem[j++];
    }
    while (i < len) { 
        A[k++] = B[i++];
    }
    delete[] B; 
}

template <typename T> void Vector<T>::mergeSort(Rank lo, Rank hi) {
    if (hi - lo < 2) return;
    Rank mi = (lo + hi) >> 1;
    mergeSort(lo, mi); 
    mergeSort(mi, hi);
    merge(lo, mi, hi); 
}

template <typename T> Rank Vector<T>::partition(Rank lo, Rank hi) {
    swap(_elem[lo], _elem[lo + rand() % (hi - lo)]); 
    T pivot = _elem[lo];
    while (lo < hi) { 
        while (lo < hi && pivot <= _elem[--hi]);
        _elem[lo] = _elem[hi];
        while (lo < hi && _elem[++lo] <= pivot);
        _elem[hi] = _elem[lo];
    }
    _elem[lo] = pivot; 
    return lo; 
}

template <typename T> void Vector<T>::quickSort(Rank lo, Rank hi) {
    if (hi - lo < 2) return; 
    Rank mi = partition(lo, hi); 
    quickSort(lo, mi); 
    quickSort(mi + 1, hi); 
}

template <typename T> void Vector<T>::heapSort(Rank lo, Rank hi) {
    // 堆排序实现
    T* A = _elem + lo;
    int n = hi - lo;

    for (int i = n / 2 - 1; i >= 0; --i) {
        int parent = i;
        int child = 2 * parent + 1;
        T temp = A[parent];
        while (child < n) {
            if (child + 1 < n && A[child] < A[child + 1]) {
                child++;
            }
            if (temp >= A[child]) break;
            A[parent] = A[child];
            parent = child;
            child = 2 * parent + 1;
        }
        A[parent] = temp;
    }

    // 堆排序
    for (int i = n - 1; i > 0; --i) {
        swap(A[0], A[i]); // 交换堆顶与最后一个元素

        int parent = 0;
        int child = 1;
        T temp = A[parent];
        while (child < i) {
            if (child + 1 < i && A[child] < A[child + 1]) {
                child++;
            }
            if (temp >= A[child]) break;
            A[parent] = A[child];
            parent = child;
            child = 2 * parent + 1;
        }
        A[parent] = temp;
    }
}

template <typename T> void Vector<T>::unsort(Rank lo, Rank hi) {
    T* V = _elem + lo;
    for (Rank i = hi - lo; i > 0; --i) {
        swap(V[i - 1], V[rand() % i]); 
    }
}



#endif  // VECTOR_H