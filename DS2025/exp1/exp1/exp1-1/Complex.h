#ifndef COMPLEX_H
#define COMPLEX_H
#include <iostream>
using namespace std;

template <typename T> class Complex {
private:
    T real;   
    T imag;  

public:
    // 构造函数
    Complex(T r = 0, T i = 0) : real(r), imag(i) {}  

    // 拷贝构造函数
    Complex(Complex<T> const& c) : real(c.real), imag(c.imag) {}

    // 赋值运算符
    Complex<T>& operator=(Complex<T> const& c) {
        if (this != &c) {
            real = c.real;
            imag = c.imag;
        }
        return *this;
    }

    // 取实部和虚部（只读）
    T getReal() const { return real; }
    T getImag() const { return imag; }

    // 设置实部和虚部
    void setReal(T r) { real = r; }
    void setImag(T i) { imag = i; }

    // 复数加法
    Complex<T> operator+(Complex<T> const& c) const {
        return Complex<T>(real + c.real, imag + c.imag);
    }

    // 复数减法
    Complex<T> operator-(Complex<T> const& c) const {
        return Complex<T>(real - c.real, imag - c.imag);
    }

    // 复数乘法
    Complex<T> operator*(Complex<T> const& c) const {
        T newReal = real * c.real - imag * c.imag;
        T newImag = real * c.imag + imag * c.real;
        return Complex<T>(newReal, newImag);
    }

    // 复数除法
    Complex<T> operator/(Complex<T> const& c) const {
        T denominator = c.real * c.real + c.imag * c.imag;  // 分母：c²+d²
        if (denominator == 0) {
            cerr << "除数不能为0！" << endl;
            exit(EXIT_FAILURE);
        }
        T newReal = (real * c.real + imag * c.imag) / denominator;
        T newImag = (imag * c.real - real * c.imag) / denominator;
        return Complex<T>(newReal, newImag);
    }

    // 复合赋值运算符
    Complex<T>& operator+=(Complex<T> const& c) {
        *this = *this + c;
        return *this;
    }
    Complex<T>& operator-=(Complex<T> const& c) {
        *this = *this - c;
        return *this;
    }
    Complex<T>& operator*=(Complex<T> const& c) {
        *this = *this * c;
        return *this;
    }
    Complex<T>& operator/=(Complex<T> const& c) {
        *this = *this / c;
        return *this;
    }

    // 比较运算符
    bool operator==(Complex<T> const& c) const {
        return (real == c.real) && (imag == c.imag);
    }
    bool operator!=(Complex<T> const& c) const {
        return !(*this == c);
    }

    // 取共轭复数
    Complex<T> conjugate() const {
        return Complex<T>(real, -imag);
    }

    // 计算模长（|a+bi| = √(a²+b²)）
    double modulus() const {
        return sqrt(real * real + imag * imag);
    }

    // 输出复数
    void print() const {
        if (imag > 0) {
            cout << real << "+" << imag << "i";
        } else if (imag < 0) {
            cout << real << imag << "i";  
        } else {
            cout << real;
        }
    }
};

// 全局输出运算符重载
template <typename T>
ostream& operator<<(ostream& os, Complex<T> const& c) {
    if (c.getImag() > 0) {
        os << c.getReal() << "+" << c.getImag() << "i";
    } else if (c.getImag() < 0) {
        os << c.getReal() << c.getImag() << "i";
    } else {
        os << c.getReal();
    }
    return os;
}



#endif  // COMPLEX_H