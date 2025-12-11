#ifndef SORT_NMS_H
#define SORT_NMS_H

#include <vector>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <iomanip>

// 边界框结构体（位置、大小、置信度）
struct BoundingBox {
    float x1;    // 左上角x
    float y1;    // 左上角y
    float x2;    // 右下角x
    float y2;    // 右下角y
    float score; // 置信度
};

// 1. 排序算法集合（按置信度降序）
void quickSort(std::vector<BoundingBox>& boxes, int left, int right);
void bubbleSort(std::vector<BoundingBox>& boxes);
void selectionSort(std::vector<BoundingBox>& boxes);
void mergeSort(std::vector<BoundingBox>& boxes);
void merge(std::vector<BoundingBox>& boxes, int left, int mid, int right);

// 2. 数据生成函数
std::vector<BoundingBox> generateRandomBoxes(int num_boxes);  // 随机分布
std::vector<BoundingBox> generateClusteredBoxes(int num_boxes); // 聚集分布

// 3. NMS算法（基础版）
std::vector<BoundingBox> nonMaximumSuppression(
    std::vector<BoundingBox> boxes, 
    int sort_algorithm, 
    float iou_threshold = 0.5f
);

// 4. 性能测试工具（返回运行时间：毫秒）
template <typename Func>
double testPerformance(Func func) {
    clock_t start = clock();
    func();
    clock_t end = clock();
    return (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;
}

// 辅助函数：计算IOU
float calculateIOU(const BoundingBox& a, const BoundingBox& b);

// 5. 实验分析函数（输出统计报告）
void printAnalysisReport(
    const double time_data[2][6][4],  // [分布][规模][排序算法]
    const int keep_count_data[2][6],  // [分布][规模] NMS保留数
    const int test_sizes[], 
    int size_count
);

#endif // SORT_NMS_H