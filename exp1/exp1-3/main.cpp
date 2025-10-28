#include <iostream>
#include <ctime>
#include "Stack.h"
#include "Vector.h"

// 计算柱状图中最大矩形面积
int largestRectangleArea(Vector<int>& heights) {
    Stack<int> stack; // 单调栈，存储柱子的索引，保持栈中索引对应的高度递增
    int maxArea = 0;
    int n = heights.size();

    for (int i = 0; i < n; ++i) {
        // 当栈不为空且当前柱子高度小于栈顶柱子高度时，弹出栈顶元素并计算面积
        while (!stack.empty() && heights[i] < heights[stack.peek()]) {
            int height = heights[stack.pop()]; // 当前弹出柱子的高度
            int width;

            if (stack.empty()) {
                width = i; // 栈为空时，宽度为当前索引
            } else {
                width = i - stack.peek() - 1; // 宽度为当前索引与新栈顶索引的差值减1
            }

            // 使用自定义max函数替代std::max
            maxArea = (maxArea > height * width) ? maxArea : height * width;
        }
        stack.push(i); // 将当前索引入栈
    }

    // 处理栈中剩余的元素
    while (!stack.empty()) {
        int height = heights[stack.pop()];
        int width;

        if (stack.empty()) {
            width = n; // 栈为空时，宽度为数组长度
        } else {
            width = n - stack.peek() - 1;
        }

        // 使用自定义max函数替代std::max
        maxArea = (maxArea > height * width) ? maxArea : height * width;
    }

    return maxArea;
}

// 生成随机测试数据
Vector<int> generateRandomHeights(int length) {
    Vector<int> heights;
    for (int i = 0; i < length; ++i) {
        heights.insert(rand() % 10001); // 生成0-10000的随机高度
    }
    return heights;
}

int main() {
    // 示例测试1
    Vector<int> example1;
    int arr1[] = {2, 1, 5, 6, 2, 3};
    int len1 = sizeof(arr1) / sizeof(arr1[0]); // 计算数组长度
    for (int i = 0; i < len1; ++i) { // 替换范围for循环
        example1.insert(arr1[i]);
    }
    cout << "示例1: ";
    example1.print();
    cout << "最大面积: " << largestRectangleArea(example1) << endl; // 应输出10

    // 示例测试2
    Vector<int> example2;
    int arr2[] = {2, 4};
    int len2 = sizeof(arr2) / sizeof(arr2[0]); // 计算数组长度
    for (int i = 0; i < len2; ++i) { // 替换范围for循环
        example2.insert(arr2[i]);
    }
    cout << "示例2: ";
    example2.print();
    cout << "最大面积: " << largestRectangleArea(example2) << endl; // 应输出4

    // 随机生成10组测试数据
    srand(time(0)); // 初始化随机数种子
    for (int i = 0; i < 10; ++i) {
        int length = rand() % 100000 + 1; // 1 <= 长度 <= 105
        Vector<int> heights = generateRandomHeights(length);
        cout << "\n测试组 " << i + 1 << " (长度: " << length << "): " << endl;
        // 只输出前10个元素（太长不适合展示）
        cout << "前10个高度: ";
        int displayLen = (length < 10) ? length : 10;
        for (int j = 0; j < displayLen; ++j) { // 替换min函数调用
            cout << heights[j] << " ";
        }
        if (length > 10) cout << "...";
        cout << endl;
        cout << "最大面积: " << largestRectangleArea(heights) << endl;
    }

    return 0;
}