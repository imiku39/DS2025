#ifndef VECTOR_H
#define VECTOR_H
#include <iostream>
#include <cstdlib>
#include <cmath>
#include "Complex.h"
#include <typeinfo>
#include <algorithm>
using namespace std;

template <typename T>
class Vector {
private:
    T* _elem;
    int _size;
    int _capacity;

    void expand() {
        if (_size < _capacity) return;
        _capacity = max(_capacity, 1) * 2;
        T* oldElem = _elem;
        _elem = new T[_capacity];
        for (int i = 0; i < _size; ++i) {
            _elem[i] = oldElem[i];
        }
        delete[] oldElem;
    }

public:
    Vector(int capacity = 1) : _size(0), _capacity(capacity) {
        _elem = new T[_capacity];
    }
    ~Vector() {
        delete[] _elem;
    }
    Vector(const Vector& v) : _size(v._size), _capacity(v._capacity) {
        _elem = new T[_capacity];
        for (int i = 0; i < _size; ++i) {
            _elem[i] = v._elem[i];
        }
    }
    Vector& operator=(const Vector& v) {
        if (this != &v) {
            delete[] _elem;
            _size = v._size;
            _capacity = v._capacity;
            _elem = new T[_capacity];
            for (int i = 0; i < _size; ++i) {
                _elem[i] = v._elem[i];
            }
        }
        return *this;
    }

    T& operator[](int idx) { return _elem[idx]; }
    const T& operator[](int idx) const { return _elem[idx]; }
    int size() const { return _size; }
    bool empty() const { return _size == 0; }
    void clear() { _size = 0; }

    int insert(int r, const T& e) {
        expand();
        for (int i = _size; i > r; --i) {
            _elem[i] = _elem[i-1];
        }
        _elem[r] = e;
        _size++;
        return r;
    }
    int push_back(const T& e) {
        return insert(_size, e);
    }

    T remove(int r) {
        T e = _elem[r];
        for (int i = r; i < _size-1; ++i) {
            _elem[i] = _elem[i+1];
        }
        _size--;
        return e;
    }
    int remove(int lo, int hi) {
        if (lo >= hi) return 0;
        while (hi < _size) {
            _elem[lo++] = _elem[hi++];
        }
        _size = lo;
        return hi - lo;
    }

    int find(const T& e, int lo = 0, int hi = -1) const {
        if (hi < 0) hi = _size;
        for (int i = hi-1; i >= lo; --i) {
            if (_elem[i] == e) {
                return i;
            }
        }
        return -1;
    }

    int deduplicate() {
        int oldSize = _size;
        int i = 1;
        while (i < _size) {
            if (find(_elem[i], 0, i) < 0) {
                i++;
            } else {
                remove(i);
            }
        }
        return oldSize - _size;
    }

    void traverse(void (*visit)(T&)) {
        for (int i = 0; i < _size; ++i) {
            visit(_elem[i]);
        }
    }
    template <typename VST>
    void traverse(VST& visit);

    void bubbleSort(int lo, int hi) {
        if (hi - lo <= 1) return;
        for (int i = lo; i < hi-1; ++i) {
            for (int j = hi-1; j > i; --j) {
                if (!Complex::compare(_elem[j-1], _elem[j])) {
                    swap(_elem[j], _elem[j-1]);
                }
            }
        }
    }
    void bubbleSort() {
        bubbleSort(0, _size);
    }

    void merge(int left, int mid, int right) {
        Vector<T> temp(right - left + 1);
        int i = left, j = mid + 1, k = 0;
        while (i <= mid && j <= right) {
            if (Complex::compare(_elem[i], _elem[j])) {
                temp[k++] = _elem[i++];
            } else {
                temp[k++] = _elem[j++];
            }
        }
        while (i <= mid) temp[k++] = _elem[i++];
        while (j <= right) temp[k++] = _elem[j++];
        for (i = left, k = 0; i <= right; i++, k++) {
            _elem[i] = temp[k];
        }
    }
    void mergeSort(int left, int right) {
        if (left < right) {
            int mid = left + (right - left) / 2;
            mergeSort(left, mid);
            mergeSort(mid + 1, right);
            merge(left, mid, right);
        }
    }
    void mergeSort() {
        mergeSort(0, _size - 1);
    }

    void print(const string& msg) const {
        cout << msg << " (size: " << _size << "): ";
        int limit = min(10, _size);
        for (int i = 0; i < limit; ++i) {
            cout << _elem[i] << " ";
        }
        if (_size > 10) cout << "...";
        cout << endl;
    }
};

template <typename T>
template <typename VST>
void Vector<T>::traverse(VST& visit) {
    for (int i = 0; i < _size; ++i) {
        visit(_elem[i]);
    }
}

#endif  // VECTOR_H