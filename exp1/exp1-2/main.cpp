#include "Stack.h"
#include <cmath>
#include <cctype>
#include <string>
#include <stdexcept>

using namespace std;

// ��������ȼ����ο����ϴ���4.6��չ��
int precedence(const string& op) {
    if (op == "sin" || op == "cos" || op == "tan" || op == "log" || op == "ln")
        return 4;  // ���Ǻ����Ͷ������ȼ����
    if (op == "^") return 3;  // ������
    if (op == "*" || op == "/") return 2;  // �˳�
    if (op == "+" || op == "-") return 1;  // �Ӽ�
    if (op == "(") return 0;  // ������
    return -1;
}

// ִ������
double calculate(double a, double b, const string& op) {
    if (op == "+") return a + b;
    if (op == "-") return b - a;  // ע��˳��b��ǰһ��������
    if (op == "*") return a * b;
    if (op == "/") {
        if (a == 0) throw runtime_error("��������Ϊ0");
        return b / a;
    }
    if (op == "^") return pow(b, a);
    return 0;
}

// ִ�е�Ŀ���㣨���Ǻ����Ͷ�����
double calculate(double a, const string& op) {
    if (op == "sin") return sin(a);
    if (op == "cos") return cos(a);
    if (op == "tan") return tan(a);
    if (op == "log") {
        if (a <= 0) throw runtime_error("log��������Ϊ����");
        return log10(a);
    }
    if (op == "ln") {
        if (a <= 0) throw runtime_error("ln��������Ϊ����");
        return log(a);
    }
    throw runtime_error("δ֪�ĵ�Ŀ�����: " + op);
}

// �ж��Ƿ�Ϊ��Ŀ�����
bool isUnaryOp(const string& op) {
    return op == "sin" || op == "cos" || op == "tan" || op == "log" || op == "ln";
}

// �������ʽ��������
double evaluate(const string& expr) {
    Stack<double> numStack;    // ������ջ
    Stack<string> opStack;     // �����ջ
    int n = expr.size();
    int i = 0;

    while (i < n) {
        // �����ո�
        if (isspace(expr[i])) {
            i++;
            continue;
        }

        // ��������
        if (isdigit(expr[i]) || expr[i] == '.') {
            double num = 0;
            // ��������
            while (i < n && isdigit(expr[i])) {
                num = num * 10 + (expr[i] - '0');
                i++;
            }
            // С������
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
        // ����������
        else if (expr[i] == '(') {
            opStack.push("(");
            i++;
        }
        // ����������
        else if (expr[i] == ')') {
            // ����ֱ������������
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
            if (opStack.empty()) throw runtime_error("���Ų�ƥ��");
            opStack.pop();  // ����������
            i++;
        }
        // ����������ͺ�����
        else if (isalpha(expr[i]) || expr[i] == '+' || expr[i] == '-' || 
                 expr[i] == '*' || expr[i] == '/' || expr[i] == '^') {
            string op;
            // ������������ĸ��
            if (isalpha(expr[i])) {
                while (i < n && isalpha(expr[i])) {
                    op += expr[i];
                    i++;
                }
                // ����Ƿ�Ϊ��Ч�ĺ�����
                if (!isUnaryOp(op)) {
                    throw runtime_error("δ֪����: " + op);
                }
            } else {
                // ���������
                op += expr[i];
                i++;
            }

            // ������������ȼ�
            while (!opStack.empty() && precedence(opStack.peek()) >= precedence(op)) {
                string topOp = opStack.pop();
                if (isUnaryOp(topOp)) {
                    double a = numStack.pop();
                    numStack.push(calculate(a, topOp));
                } else {
                    if (numStack.size() < 2) throw runtime_error("���ʽ��Ч");
                    double a = numStack.pop();
                    double b = numStack.pop();
                    numStack.push(calculate(a, b, topOp));
                }
            }
            opStack.push(op);
        } else {
            throw runtime_error("��Ч�ַ�: " + string(1, expr[i]));
        }
    }

    // ����ʣ��������
    while (!opStack.empty()) {
        string op = opStack.pop();
        if (op == "(") throw runtime_error("���Ų�ƥ��");
        
        if (isUnaryOp(op)) {
            if (numStack.empty()) throw runtime_error("���ʽ��Ч");
            double a = numStack.pop();
            numStack.push(calculate(a, op));
        } else {
            if (numStack.size() < 2) throw runtime_error("���ʽ��Ч");
            double a = numStack.pop();
            double b = numStack.pop();
            numStack.push(calculate(a, b, op));
        }
    }

    if (numStack.size() != 1) throw runtime_error("���ʽ��Ч");
    return numStack.pop();
}

// ���԰���
int main() {
    // �����������
    try {
        string expr1 = "3 + 4 * 2 / (1 - 5)";
        cout << expr1 << " = " << evaluate(expr1) << endl;  // Ӧ���1
        
        string expr2 = "2 + 3 * 4";
        cout << expr2 << " = " << evaluate(expr2) << endl;  // Ӧ���14
        
        string expr3 = "10 + 20 / (5 - 3)";
        cout << expr3 << " = " << evaluate(expr3) << endl;  // Ӧ���20
    } catch (const exception& e) {
        cout << "����: " << e.what() << endl;
    }

    // �����������
    try {
        string expr4 = "sin(3.1415926535 / 2)";
        cout << expr4 << " = " << evaluate(expr4) << endl;  // Ӧ�ӽ�1
        
        string expr5 = "log(100) + ln(1)";
        cout << expr5 << " = " << evaluate(expr5) << endl;  // Ӧ���2
        
        string expr6 = "2 ^ 3 + tan(0)";
        cout << expr6 << " = " << evaluate(expr6) << endl;  // Ӧ���8
    } catch (const exception& e) {
        cout << "����: " << e.what() << endl;
    }

    // ����������
    try {
        string expr7 = "3 + 4 * (2 - 5";  // ���Ų�ƥ��
        cout << expr7 << " = " << evaluate(expr7) << endl;
    } catch (const exception& e) {
        cout << "����: " << e.what() << endl;  // Ӧ������Ų�ƥ��
    }

    try {
        string expr8 = "10 / 0";  // �������
        cout << expr8 << " = " << evaluate(expr8) << endl;
    } catch (const exception& e) {
        cout << "����: " << e.what() << endl;  // Ӧ�����������Ϊ0
    }

    return 0;
}
