#include "Stack.h"
#include <cmath>
#include <cctype>
#include <string>
#include <stdexcept>

using namespace std;

// 运算符优先级表（参考书上代码4.6扩展）
int precedence(const string& op) {
    if (op == "sin" || op == "cos" || op == "tan" || op == "log" || op == "ln")
        return 4;  // 三角函数和对数优先级最高
    if (op == "^") return 3;  // 幂运算
    if (op == "*" || op == "/") return 2;  // 乘除
    if (op == "+" || op == "-") return 1;  // 加减
    if (op == "(") return 0;  // 左括号
    return -1;
}

// 执行运算
double calculate(double a, double b, const string& op) {
    if (op == "+") return a + b;
    if (op == "-") return b - a;  // 注意顺序，b是前一个操作数
    if (op == "*") return a * b;
    if (op == "/") {
        if (a == 0) throw runtime_error("除数不能为0");
        return b / a;
    }
    if (op == "^") return pow(b, a);
    return 0;
}

// 执行单目运算（三角函数和对数）
double calculate(double a, const string& op) {
    if (op == "sin") return sin(a);
    if (op == "cos") return cos(a);
    if (op == "tan") return tan(a);
    if (op == "log") {
        if (a <= 0) throw runtime_error("log参数必须为正数");
        return log10(a);
    }
    if (op == "ln") {
        if (a <= 0) throw runtime_error("ln参数必须为正数");
        return log(a);
    }
    throw runtime_error("未知的单目运算符: " + op);
}

// 判断是否为单目运算符
bool isUnaryOp(const string& op) {
    return op == "sin" || op == "cos" || op == "tan" || op == "log" || op == "ln";
}

// 解析表达式并计算结果
double evaluate(const string& expr) {
    Stack<double> numStack;    // 操作数栈
    Stack<string> opStack;     // 运算符栈
    int n = expr.size();
    int i = 0;

    while (i < n) {
        // 跳过空格
        if (isspace(expr[i])) {
            i++;
            continue;
        }

        // 处理数字
        if (isdigit(expr[i]) || expr[i] == '.') {
            double num = 0;
            // 整数部分
            while (i < n && isdigit(expr[i])) {
                num = num * 10 + (expr[i] - '0');
                i++;
            }
            // 小数部分
            if (i < n && expr[i] == '.') {
                i++;
                double frac = 0.1;
                while (i < n && isdigit(expr[i])) {
                    num += (expr[i] - '0') * frac;
                    frac *= 0.1;
                    i++;
                }
            }
            numStack.push(num);
        }
        // 处理左括号
        else if (expr[i] == '(') {
            opStack.push("(");
            i++;
        }
        // 处理右括号
        else if (expr[i] == ')') {
            // 弹出直到遇到左括号
            while (!opStack.empty() && opStack.peek() != "(") {
                string op = opStack.pop();
                if (isUnaryOp(op)) {
                    double a = numStack.pop();
                    numStack.push(calculate(a, op));
                } else {
                    double a = numStack.pop();
                    double b = numStack.pop();
                    numStack.push(calculate(a, b, op));
                }
            }
            if (opStack.empty()) throw runtime_error("括号不匹配");
            opStack.pop();  // 弹出左括号
            i++;
        }
        // 处理运算符和函数名
        else if (isalpha(expr[i]) || expr[i] == '+' || expr[i] == '-' || 
                 expr[i] == '*' || expr[i] == '/' || expr[i] == '^') {
            string op;
            // 处理函数名（字母）
            if (isalpha(expr[i])) {
                while (i < n && isalpha(expr[i])) {
                    op += expr[i];
                    i++;
                }
                // 检查是否为有效的函数名
                if (!isUnaryOp(op)) {
                    throw runtime_error("未知函数: " + op);
                }
            } else {
                // 处理运算符
                op += expr[i];
                i++;
            }

            // 处理运算符优先级
            while (!opStack.empty() && precedence(opStack.peek()) >= precedence(op)) {
                string topOp = opStack.pop();
                if (isUnaryOp(topOp)) {
                    double a = numStack.pop();
                    numStack.push(calculate(a, topOp));
                } else {
                    if (numStack.size() < 2) throw runtime_error("表达式无效");
                    double a = numStack.pop();
                    double b = numStack.pop();
                    numStack.push(calculate(a, b, topOp));
                }
            }
            opStack.push(op);
        } else {
            throw runtime_error("无效字符: " + string(1, expr[i]));
        }
    }

    // 处理剩余的运算符
    while (!opStack.empty()) {
        string op = opStack.pop();
        if (op == "(") throw runtime_error("括号不匹配");
        
        if (isUnaryOp(op)) {
            if (numStack.empty()) throw runtime_error("表达式无效");
            double a = numStack.pop();
            numStack.push(calculate(a, op));
        } else {
            if (numStack.size() < 2) throw runtime_error("表达式无效");
            double a = numStack.pop();
            double b = numStack.pop();
            numStack.push(calculate(a, b, op));
        }
    }

    if (numStack.size() != 1) throw runtime_error("表达式无效");
    return numStack.pop();
}

// 测试案例
int main() {
    // 基本运算测试
    try {
        string expr1 = "3 + 4 * 2 / (1 - 5)";
        cout << expr1 << " = " << evaluate(expr1) << endl;  // 应输出1
        
        string expr2 = "2 + 3 * 4";
        cout << expr2 << " = " << evaluate(expr2) << endl;  // 应输出14
        
        string expr3 = "10 + 20 / (5 - 3)";
        cout << expr3 << " = " << evaluate(expr3) << endl;  // 应输出20
    } catch (const exception& e) {
        cout << "错误: " << e.what() << endl;
    }

    // 函数运算测试
    try {
        string expr4 = "sin(3.1415926535 / 2)";
        cout << expr4 << " = " << evaluate(expr4) << endl;  // 应接近1
        
        string expr5 = "log(100) + ln(1)";
        cout << expr5 << " = " << evaluate(expr5) << endl;  // 应输出2
        
        string expr6 = "2 ^ 3 + tan(0)";
        cout << expr6 << " = " << evaluate(expr6) << endl;  // 应输出8
    } catch (const exception& e) {
        cout << "错误: " << e.what() << endl;
    }

    // 错误案例测试
    try {
        string expr7 = "3 + 4 * (2 - 5";  // 括号不匹配
        cout << expr7 << " = " << evaluate(expr7) << endl;
    } catch (const exception& e) {
        cout << "错误: " << e.what() << endl;  // 应输出括号不匹配
    }

    try {
        string expr8 = "10 / 0";  // 除零错误
        cout << expr8 << " = " << evaluate(expr8) << endl;
    } catch (const exception& e) {
        cout << "错误: " << e.what() << endl;  // 应输出除数不能为0
    }

    return 0;
}
