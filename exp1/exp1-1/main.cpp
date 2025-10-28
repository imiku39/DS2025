#include <iostream>
#include <ctime>
#include <cmath>
#include <sstream>
#include "Complex.h"
#include "Vector.h"

using namespace std;

// 生成随机复数
Complex generateRandomComplex() {
    // 简单的随机数生成器
    static unsigned long seed = 1;
    seed = seed * 1103515245 + 12345;
    double real_part = -10.0 + (seed % 20000) / 1000.0; // 范围 [-10, 10]
    seed = seed * 1103515245 + 12345;
    double imag_part = -10.0 + (seed % 20000) / 1000.0; // 范围 [-10, 10]
    return Complex(real_part, imag_part);
}

// 生成随机复数向量
Vector<Complex> generateRandomVector(int size) {
    Vector<Complex> vec;
    for (int i = 0; i < size; ++i) {
        vec.push_back(generateRandomComplex());
    }
    
    // 添加一些重复项
    for (int i = 0; i < size / 5; ++i) {
        int idx = rand() % size;
        vec.push_back(vec[idx]);
    }
    
    return vec;
}

// 区间查找算法：查找出模介于[m1,m2)的所有元素
Vector<Complex> rangeSearch(const Vector<Complex>& vec, double m1, double m2) {
    Vector<Complex> result;
    for (int i = 0; i < vec.size(); ++i) {
        double modulus = vec[i].norm();
        if (modulus >= m1 && modulus < m2) {
            result.push_back(vec[i]);
        }
    }
    return result;
}

// 冒泡排序计时
clock_t bubbleSortTime(Vector<Complex>& vec) {
    clock_t start = clock();
    vec.bubbleSort();
    clock_t end = clock();
    return end - start;
}

// 归并排序计时
clock_t mergeSortTime(Vector<Complex>& vec) {
    clock_t start = clock();
    vec.mergeSort();
    clock_t end = clock();
    return end - start;
}

// 自定义to_string函数
string my_to_string(double value) {
    ostringstream oss;
    oss << value;
    return oss.str();
}

int main() {
    // 随机种子
    srand(time(0));
    
    cout << "=== 复数向量操作测试 ===" << endl;
    
    // 生成随机复数向量
    Vector<Complex> vec = generateRandomVector(20);
    vec.print("原始向量");
    
    // 测试查找（实部和虚部均相同）
    Complex target(1.0, 2.0);
    cout << "查找 " << target << ": ";
    int pos = vec.find(target);
    if (pos != -1) {
        cout << "找到，位置: " << pos << endl;
    } else {
        cout << "未找到" << endl;
    }
    
    // 测试插入
    Complex insert_elem(5.5, 6.6);
    vec.insert(0, insert_elem);
    vec.print("插入元素后");
    
    // 测试删除
    vec.remove(0);
    vec.print("删除第0个元素后");
    
    // 测试唯一化
    Vector<Complex> vec_dedup = vec;
    vec_dedup.print("唯一化前");
    int removed = vec_dedup.deduplicate();
    vec_dedup.print("唯一化后");
    cout << "移除了 " << removed << " 个重复元素" << endl;
    
    cout << "\n=== 排序性能测试 ===" << endl;
    
    // 生成大一些的向量用于性能测试
    Vector<Complex> large_vec = generateRandomVector(1000);
    
    // 顺序向量
    Vector<Complex> ordered_vec = large_vec;
    ordered_vec.mergeSort(); // 先用归并排序得到有序向量
    cout << "顺序向量排序性能测试:" << endl;
    
    Vector<Complex> bubble_ordered = ordered_vec;
    clock_t bubble_ordered_time = bubbleSortTime(bubble_ordered);
    cout << "冒泡排序时间: " << bubble_ordered_time << " 时钟周期" << endl;
    
    Vector<Complex> merge_ordered = ordered_vec;
    clock_t merge_ordered_time = mergeSortTime(merge_ordered);
    cout << "归并排序时间: " << merge_ordered_time << " 时钟周期" << endl;
    
    // 乱序向量
    Vector<Complex> unordered_vec = large_vec;
    // 通过多次随机交换来打乱顺序
    for (int i = 0; i < 2000; ++i) {
        int idx1 = rand() % unordered_vec.size();
        int idx2 = rand() % unordered_vec.size();
        Complex temp = unordered_vec[idx1];
        unordered_vec[idx1] = unordered_vec[idx2];
        unordered_vec[idx2] = temp;
    }
    cout << "\n乱序向量排序性能测试:" << endl;
    
    Vector<Complex> bubble_unordered = unordered_vec;
    clock_t bubble_unordered_time = bubbleSortTime(bubble_unordered);
    cout << "冒泡排序时间: " << bubble_unordered_time << " 时钟周期" << endl;
    
    Vector<Complex> merge_unordered = unordered_vec;
    clock_t merge_unordered_time = mergeSortTime(merge_unordered);
    cout << "归并排序时间: " << merge_unordered_time << " 时钟周期" << endl;
    
    // 逆序向量
    Vector<Complex> reversed_vec = ordered_vec;
    for (int i = 0; i < reversed_vec.size() / 2; ++i) {
        Complex temp = reversed_vec[i];
        reversed_vec[i] = reversed_vec[reversed_vec.size() - 1 - i];
        reversed_vec[reversed_vec.size() - 1 - i] = temp;
    }
    cout << "\n逆序向量排序性能测试:" << endl;
    
    Vector<Complex> bubble_reversed = reversed_vec;
    clock_t bubble_reversed_time = bubbleSortTime(bubble_reversed);
    cout << "冒泡排序时间: " << bubble_reversed_time << " 时钟周期" << endl;
    
    Vector<Complex> merge_reversed = reversed_vec;
    clock_t merge_reversed_time = mergeSortTime(merge_reversed);
    cout << "归并排序时间: " << merge_reversed_time << " 时钟周期" << endl;
    
    cout << "\n=== 区间查找测试 ===" << endl;
    
    // 对向量进行排序以进行区间查找
    Vector<Complex> search_vec = generateRandomVector(30);
    search_vec.mergeSort();
    search_vec.print("已排序向量");
    
    // 查找模在[2.0, 5.0)之间的元素
    double m1 = 2.0, m2 = 5.0;
    Vector<Complex> range_result = rangeSearch(search_vec, m1, m2);
    cout << "模在 [" << m1 << ", " << m2 << ") 之间的元素: ";
    range_result.print("区间查找结果");
    
    return 0;
}



