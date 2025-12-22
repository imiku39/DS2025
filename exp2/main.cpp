#include <iostream>
#include <fstream>
#include <queue>
#include <map>
#include <vector>
#include <cctype>
#include <cstddef>
#include <cstring>
#include <string>

using namespace std;

// 修复后的位图类：新增编码长度记录，区分"有效位数量"和"编码总长度"
class Bitmap {
private:
    unsigned char* M;
    size_t N;          // 字节数
    size_t bit_length; // 编码总长度（关键：记录0/1的总位数）
    size_t _sz;        // 有效1的数量

protected:
    void init(size_t n) {
        M = new unsigned char[N = (n + 7) / 8];
        memset(M, 0, N);
        bit_length = 0; // 初始编码长度为0
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
        // 重新计算有效1的数量和编码长度（这里编码长度默认取n）
        _sz = 0;
        bit_length = n;
        for (size_t k = 0; k < n; k++)
            if (test(k))
                _sz++;
    }

    // 析构函数
    ~Bitmap() {
        delete[] M;
        M = NULL;
        bit_length = 0;
        _sz = 0;
    }

    // 拷贝构造函数
    Bitmap(const Bitmap& other) {
        init(other.N * 8);
        memcpy(M, other.M, other.N);
        bit_length = other.bit_length;
        _sz = other._sz;
    }

    // 赋值运算符
    Bitmap& operator=(const Bitmap& other) {
        if (this != &other) {
            delete[] M;
            init(other.N * 8);
            memcpy(M, other.M, other.N);
            bit_length = other.bit_length;
            _sz = other._sz;
        }
        return *this;
    }

    // 获取编码总长度（关键：返回0/1的总位数）
    size_t length() const { return bit_length; }
    // 获取有效1的数量（保留原逻辑）
    size_t size() const { return _sz; }

    // 设置第k位为1，并更新编码长度
    void set(size_t k) {
        expand(k);
        if (!test(k)) {
            _sz++;
            M[k >> 3] |= (0x80 >> (k & 0x07));
        }
        // 确保编码长度覆盖到当前位
        if (k + 1 > bit_length) {
            bit_length = k + 1;
        }
    }

    // 清除第k位为0，并更新编码长度
    void clear(size_t k) {
        expand(k);
        if (test(k)) {
            _sz--;
            M[k >> 3] &= ~(0x80 >> (k & 0x07));
        }
        // 确保编码长度覆盖到当前位
        if (k + 1 > bit_length) {
            bit_length = k + 1;
        }
    }

    // 测试第k位的值
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

    // 修正：按编码总长度转换为字符串
    char* bits2string() const {
        char* s = new char[bit_length + 1];
        s[bit_length] = '\0';
        for (size_t i = 0; i < bit_length; i++) {
            s[i] = test(i) ? '1' : '0';
        }
        return s;
    }
};

// Huffman编码类型定义
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
            freq[lower_c]++; // 简化逻辑：直接++，无需判断是否存在
        }
    }
    return freq;
}

// 构建Huffman树
template <typename T>
HuffNode<T>* buildHuffTree(const map<T, int>& freq) {
    if (freq.empty()) return NULL;

    priority_queue<HuffNode<T>*, vector<HuffNode<T>*>, HuffNodeCompare<T> > pq;

    // 创建叶子节点
    for (typename map<T, int>::const_iterator it = freq.begin(); it != freq.end(); ++it) {
        pq.push(new HuffNode<T>(it->first, it->second));
    }

    // 构建Huffman树
    while (pq.size() > 1) {
        HuffNode<T>* left = pq.top(); pq.pop();
        HuffNode<T>* right = pq.top(); pq.pop();

        // 创建内部节点（数据域用空字符标识）
        HuffNode<T>* parent = new HuffNode<T>('\0', left->weight + right->weight, left, right);
        pq.push(parent);
    }

    return pq.top();
}

// 修复后的生成Huffman编码表（递归遍历树）
template <typename T>
void generateHuffCodes(HuffNode<T>* root, map<T, HuffCode>& codes, HuffCode currentCode, int depth) {
    if (!root) return;

    // 叶子节点：记录编码
    if (!root->left && !root->right) {
        codes[root->data] = currentCode;
        return;
    }

    // 左子树：添加0（clear当前depth位）
    currentCode.clear(depth);
    generateHuffCodes(root->left, codes, currentCode, depth + 1);

    // 右子树：添加1（set当前depth位）
    currentCode.set(depth);
    generateHuffCodes(root->right, codes, currentCode, depth + 1);
}

// 编码字符串
string encodeString(const string& str, const map<char, HuffCode>& codes) {
    string result;
    for (size_t i = 0; i < str.size(); ++i) {
        char c = str[i];
        char lower_c = tolower(static_cast<unsigned char>(c));
        map<char, HuffCode>::const_iterator it = codes.find(lower_c);
        if (it != codes.end()) {
            char* bits = it->second.bits2string(); // 使用修复后的方法
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
        // 修复：传递depth=0，追踪编码的深度（位数）
        generateHuffCodes(huffRoot, huffCodes, HuffCode(), 0);
    }

    // 输出Huffman编码表
    cout << "Huffman编码表:" << endl;
    for (map<char, HuffCode>::const_iterator it = huffCodes.begin(); it != huffCodes.end(); ++it) {
        cout << "'" << it->first << "': ";
        char* bits = it->second.bits2string(); // 无需传长度，使用内部记录的length
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