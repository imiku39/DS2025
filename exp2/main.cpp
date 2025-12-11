#include <iostream>
#include <fstream>
#include <queue>
#include <map>
#include <vector>
#include <cctype>
#include <cstddef>
#include <cstring>
#include <string>
#include "Binary_tree.h"

using namespace std;

// 位图类定义（确保在HuffCode前定义）
class Bitmap {
    private:
        unsigned char* M;
        size_t N, _sz; // N: 字节数, _sz: 有效位数量

    protected:
        void init(size_t n) {
            M = new unsigned char[N = (n + 7) / 8];
            memset(M, 0, N);
            _sz = 0;
        }

        void expand(size_t k) {
            if (k < 8 * N) return;
            size_t oldN = N;
            unsigned char* oldM = M;
            init(2 * k);  // 扩展为足够容纳k位的大小
            memcpy(M, oldM, oldN);
            delete[] oldM;
        }

    public:
        // 构造函数
        Bitmap(size_t n = 8) { init(n); }
    
        Bitmap(const char* file, size_t n = 8) {
            init(n);
            FILE* fp = fopen(file, "r");
            if (fp) {
                fread(M, sizeof(char), N, fp);
                fclose(fp);
            }
            // 计算有效位数量
            _sz = 0;
            for (size_t k = 0; k < n; k++)
                if (test(k))
                    _sz++;
        }
    
        // 析构函数
        ~Bitmap() {
            delete[] M;
            M = NULL;
            _sz = 0;
        }
    
        // 拷贝构造函数（解决编码时的浅拷贝问题）
        Bitmap(const Bitmap& other) {
            init(other.N * 8);
            memcpy(M, other.M, other.N);
            _sz = other._sz;
        }
    
        // 赋值运算符（解决编码时的浅拷贝问题）
        Bitmap& operator=(const Bitmap& other) {
            if (this != &other) {
                delete[] M;
                init(other.N * 8);
                memcpy(M, other.M, other.N);
                _sz = other._sz;
            }
            return *this;
        }
    
        size_t size() const { return _sz; }
    
        void set(size_t k) {
            expand(k);
            if (!test(k)) {  // 避免重复设置同一位置
                _sz++;
                M[k >> 3] |= (0x80 >> (k & 0x07));
            }
        }
    
        void clear(size_t k) {
            expand(k);
            if (test(k)) {  // 避免重复清除同一位置
                _sz--;
                M[k >> 3] &= ~(0x80 >> (k & 0x07));
            }
        }
    
        bool test(size_t k) const {
            if (k >= 8 * N) return false;
            return M[k >> 3] & (0x80 >> (k & 0x07));
        }
    
        void dump(const char* file) const {
            FILE* fp = fopen(file, "w");
            if (fp) {
                fwrite(M, sizeof(char), N, fp);
                fclose(fp);
            }
        }
    
        // const-qualified bits2string: does NOT modify the Bitmap (no expand())
        char* bits2string(size_t n) const {
            // Do not call expand here because this method is const.
            // If requested length exceeds current storage, treat extra bits as 0.
            size_t usable = (n <= 8 * N) ? n : 8 * N;
            char* s = new char[n + 1];
            s[n] = '\0';
            for (size_t i = 0; i < n; i++)
                s[i] = (i < usable && test(i)) ? '1' : '0';
            return s;
        }
};

// Huffman编码类型定义（在Bitmap后定义）
typedef Bitmap HuffCode;

// Huffman树节点结构
template <typename T>
struct HuffNode {
    T data;          // 字符
    int weight;      // 权重(频率)
    HuffNode *left, *right; // 左右子树

    HuffNode(T d, int w, HuffNode* l = NULL, HuffNode* r = NULL)
        : data(d), weight(w), left(l), right(r) {}
};

// 比较器用于优先队列（小顶堆）
template <typename T>
struct HuffNodeCompare {
    bool operator()(HuffNode<T>* a, HuffNode<T>* b) {
        return a->weight > b->weight;
    }
};

// 从文本计算字符频率（仅统计字母）
map<char, int> calculateFrequency(const string& text) {
    map<char, int> freq;
    for (size_t i = 0; i < text.size(); ++i) {
        char c = text[i];
        if (isalpha(static_cast<unsigned char>(c))) {
            char lower_c = tolower(static_cast<unsigned char>(c));
            if (freq.find(lower_c) == freq.end())
                freq[lower_c] = 1;
            else
                freq[lower_c]++;
        }
    }
    return freq;
}

// 构建Huffman树
template <typename T>
HuffNode<T>* buildHuffTree(const map<T, int>& freq) {
    if (freq.empty()) return NULL;

    priority_queue<HuffNode<T>*, vector<HuffNode<T>*>, HuffNodeCompare<T> > pq;

    // 创建叶子节点（修正迭代器声明）
    for (typename map<T, int>::const_iterator it = freq.begin(); it != freq.end(); ++it) { 
        const pair<T, int>& nodePair = *it;  // 显式声明pair类型
        pq.push(new HuffNode<T>(nodePair.first, nodePair.second));
    }

    // 构建Huffman树（其余代码不变）
    while (pq.size() > 1) {
        HuffNode<T>* left = pq.top(); pq.pop();
        HuffNode<T>* right = pq.top(); pq.pop();
        
        // 创建内部节点（数据域用空字符标识）
        HuffNode<T>* parent = new HuffNode<T>('\0', left->weight + right->weight, left, right);
        pq.push(parent);
    }

    return pq.top();
}

// 生成Huffman编码表（递归遍历树）
template <typename T>
void generateHuffCodes(HuffNode<T>* root, map<T, HuffCode>& codes, HuffCode currentCode) {
    if (!root) return;

    // 叶子节点：记录编码
    if (!root->left && !root->right) {
        codes[root->data] = currentCode;
        return;
    }

    // 左子树：添加0（通过不设置当前位表示0）
    size_t currentSize = currentCode.size();
    currentCode.set(currentSize);  // 占位（后续会清除）
    generateHuffCodes(root->left, codes, currentCode);
    currentCode.clear(currentSize);  // 清除当前位表示0

    // 右子树：添加1（设置当前位表示1）
    currentCode.set(currentSize);
    generateHuffCodes(root->right, codes, currentCode);
}

// 编码字符串
string encodeString(const string& str, const map<char, Bitmap>& codes) {
    string result;
    for (size_t i = 0; i < str.size(); ++i) {
        char c = str[i];
        char lower_c = tolower(static_cast<unsigned char>(c));
        map<char, Bitmap>::const_iterator it = codes.find(lower_c);
        if (it != codes.end()) {
            char* bits = it->second.bits2string(it->second.size());
            result += bits;
            delete[] bits;
        }
    }
    return result;
}

// 释放Huffman树内存
template <typename T>
void destroyHuffTree(HuffNode<T>* root) {
    if (root) {
        destroyHuffTree(root->left);
        destroyHuffTree(root->right);
        delete root;
    }
}

int main() {
    // 测试文本：《I have a dream》演讲片段
    string speech = "I have a dream that one day this nation will rise up and live out the true meaning of its creed: "
                    "We hold these truths to be self-evident, that all men are created equal. "
                    "I have a dream that one day on the red hills of Georgia, the sons of former slaves and the sons "
                    "of former slave owners will be able to sit down together at the table of brotherhood. "
                    "I have a dream that my four little children will one day live in a nation where they will not be "
                    "judged by the color of their skin but by the content of their character. "
                    "I have a dream today!";

    // 计算字符频率
    map<char, int> freq = calculateFrequency(speech);

    // 输出字符频率
    cout << "字符频率统计:" << endl;
    for (map<char, int>::const_iterator it = freq.begin(); it != freq.end(); ++it) {
        cout << "'" << it->first << "': " << it->second << endl;
    }
    cout << endl;

    // 构建Huffman树
    HuffNode<char>* huffRoot = buildHuffTree<char>(freq);
    if (!huffRoot) {
        cerr << "无法构建Huffman树(空频率表)" << endl;
        return 1;
    }

    // 生成Huffman编码
    map<char, HuffCode> huffCodes;
    // 处理只有一个字符的特殊情况
    if (!huffRoot->left && !huffRoot->right) {
        HuffCode code(1);
        code.set(0);  // 单个字符编码为"1"
        huffCodes[huffRoot->data] = code;
    } else {
        generateHuffCodes(huffRoot, huffCodes, HuffCode());
    }

    // 输出Huffman编码表
    cout << "Huffman编码表:" << endl;
    for (map<char, HuffCode>::const_iterator it = huffCodes.begin(); it != huffCodes.end(); ++it) {
        cout << "'" << it->first << "': ";
        char* bits = it->second.bits2string(it->second.size());
        cout << bits << endl;
        delete[] bits;
    }
    cout << endl;

    // 编码测试
    vector<string> testWords;
    testWords.push_back("dream");
    testWords.push_back("nation");
    testWords.push_back("equal");
    testWords.push_back("freedom");
    testWords.push_back("justice");

    for (size_t wi = 0; wi < testWords.size(); ++wi) {
        const string& word = testWords[wi];
        string encoded = encodeString(word, huffCodes);
        cout << "单词 '" << word << "' 的编码: " << encoded << endl;
    }

    // 释放资源
    destroyHuffTree(huffRoot);
    return 0;
}