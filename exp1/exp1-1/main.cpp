#include "Vector.h"
#include "Complex.h"
#include <ctime>
#include <iostream>
#include <cmath>  
using namespace std;

// 生成随机复数（实部和虚部范围：[-100, 100)）
Complex<int> generateRandomComplex() {
    int real = rand() % 200 - 100;  // -100到99
    int imag = rand() % 200 - 100;  // -100到99
    return Complex<int>(real, imag);
}

// 生成指定大小的随机复数向量（含重复项）
Vector<Complex<int> > generateRandomVector(int size) {
    Vector<Complex<int> > vec;
    for (int i = 0; i < size; ++i) {
        if (i > 0 && rand() % 10 < 3) {
            vec.insert(vec[rand() % i]); 
        } else {
            vec.insert(generateRandomComplex());
        }
    }
    return vec;
}

// 测试无序向量的基本操作
void testUnorderedOperations() {
    cout << "=== 测试无序向量操作 ===" << endl;
    Vector<Complex<int> > vec = generateRandomVector(10);
    
    cout << "初始向量: ";
    vec.print();
    
    // 测试置乱
    vec.unsort();
    cout << "置乱后:   ";
    vec.print();
    
    // 测试查找
    if (!vec.empty()) {
        Complex<int> target = vec[rand() % vec.size()];
        cout << "查找 " << target << ": 秩为 " << vec.find(target) << endl;
        
        Complex<int> nonExistent(999, 999);
        Rank pos = vec.find(nonExistent);
        cout << "查找 " << nonExistent << ": ";
        if (pos < 0) {
            cout << "不存在";
        } else {
            cout << "秩为 " << pos;
        }
        cout << endl;
    }
    
    // 测试插入
    Complex<int> newElem(10, 20);
    vec.insert(3, newElem);
    cout << "插入 " << newElem << " 到秩3后: ";
    vec.print();
    
    // 测试删除
    if (vec.size() > 0) {
        Rank delRank = rand() % vec.size();
        Complex<int> delElem = vec[delRank];
        vec.remove(delRank);
        cout << "删除秩" << delRank << "的" << delElem << "后: ";
        vec.print();
    }
    
    // 测试去重
    int removed = vec.deduplicate();
    cout << "去重后(删除了" << removed << "个元素): ";
    vec.print();
    cout << endl;
}

// 测试排序效率
void testSortingEfficiency() {
    cout << "=== 测试排序效率 ===" << endl;
    const int SIZE = 100;  // 测试规模
    Vector<Complex<int> > original = generateRandomVector(SIZE);
    
    // 生成三种序列：顺序、乱序、逆序
    Vector<Complex<int> > sortedVec(original);
    sortedVec.sort();  // 顺序
    
    Vector<Complex<int> > randomVec(original);  // 乱序
    
    Vector<Complex<int> > reversedVec(sortedVec);
    // 生成逆序
    for (int i = 0; i < reversedVec.size() / 2; ++i) {
        swap(reversedVec[i], reversedVec[reversedVec.size() - 1 - i]);
    }
    
    // 测试冒泡排序
    cout << "冒泡排序:" << endl;
    Vector<Complex<int> > testVec = sortedVec;
    clock_t start = clock();
    testVec.bubbleSort(0, testVec.size());
    clock_t end = clock();
    cout << "  顺序: " << (double)(end - start) / CLOCKS_PER_SEC << "秒" << endl;
    
    testVec = randomVec;
    start = clock();
    testVec.bubbleSort(0, testVec.size());
    end = clock();
    cout << "  乱序: " << (double)(end - start) / CLOCKS_PER_SEC << "秒" << endl;
    
    testVec = reversedVec;
    start = clock();
    testVec.bubbleSort(0, testVec.size());
    end = clock();
    cout << "  逆序: " << (double)(end - start) / CLOCKS_PER_SEC << "秒" << endl;
    
    // 测试归并排序
    cout << "归并排序:" << endl;
    testVec = sortedVec;
    start = clock();
    testVec.mergeSort(0, testVec.size());
    end = clock();
    cout << "  顺序: " << (double)(end - start) / CLOCKS_PER_SEC << "秒" << endl;
    
    testVec = randomVec;
    start = clock();
    testVec.mergeSort(0, testVec.size());
    end = clock();
    cout << "  乱序: " << (double)(end - start) / CLOCKS_PER_SEC << "秒" << endl;
    
    testVec = reversedVec;
    start = clock();
    testVec.mergeSort(0, testVec.size());
    end = clock();
    cout << "  逆序: " << (double)(end - start) / CLOCKS_PER_SEC << "秒" << endl;
    cout << endl;
}

// 在有序向量中查找模介于[m1, m2)的元素
Vector<Complex<int> > findByModulusRange(const Vector<Complex<int> >& sortedVec, double m1, double m2) {
    Vector<Complex<int> > result;
    if (sortedVec.empty()) return result;
    
    // 找到第一个模 >= m1的元素
    Rank left = 0, right = sortedVec.size();
    while (left < right) {
        Rank mid = (left + right) >> 1;
        if (sortedVec[mid].modulus() >= m1) {
            right = mid;
        } else {
            left = mid + 1;
        }
    }
    Rank start = left;
    
    // 找到第一个模 >= m2的元素
    left = 0;
    right = sortedVec.size();
    while (left < right) {
        Rank mid = (left + right) >> 1;
        if (sortedVec[mid].modulus() >= m2) {
            right = mid;
        } else {
            left = mid + 1;
        }
    }
    Rank end = left;
    
    // 提取区间元素
    for (Rank i = start; i < end; ++i) {
        result.insert(sortedVec[i]);
    }
    return result;
}

// 测试区间查找功能
void testRangeSearch() {
    cout << "=== 测试区间查找 ===" << endl;
    Vector<Complex<int> > vec = generateRandomVector(20);
    vec.sort();  // 确保向量有序
    cout << "有序向量: ";
    vec.print();
    
    // 查找模在[50, 100)之间的元素
    double m1 = 50, m2 = 100;
    Vector<Complex<int> > rangeVec = findByModulusRange(vec, m1, m2);
    
    cout << "模介于[" << m1 << "," << m2 << ")的元素: ";
    if (rangeVec.empty()) {
        cout << "无符合条件的元素";
    } else {
        rangeVec.print();
    }
    cout << endl;
}

int main() {
    srand((unsigned int)time(NULL));  // 初始化随机数种子
    
    testUnorderedOperations();
    testSortingEfficiency();
    testRangeSearch();
    
    return 0;
}