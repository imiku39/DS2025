#ifndef COMPLEX_H
#define COMPLEX_H
#include <iostream>
#include <cmath>
using namespace std;

class Complex {
private:
    double _real;  // 实部
    double _imag;  // 虚部

public:
    Complex(double real = 0, double imag = 0) : _real(real), _imag(imag) {}
    Complex(const Complex& c) : _real(c._real), _imag(c._imag) {}
    Complex& operator=(const Complex& c) {
        if (this != &c) {
            _real = c._real;
            _imag = c._imag;
        }
        return *this;
    }

    double real() const { return _real; }
    double imag() const { return _imag; }
    void setReal(double real) { _real = real; }
    void setImag(double imag) { _imag = imag; }

    Complex add(const Complex& c) const {
        return Complex(_real + c._real, _imag + c._imag);
    }
    Complex subtract(const Complex& c) const {
        return Complex(_real - c._real, _imag - c._imag);
    }
    Complex multiply(const Complex& c) const {
        double realPart = _real * c._real - _imag * c._imag;
        double imagPart = _imag * c._real + _real * c._imag;
        return Complex(realPart, imagPart);
    }
    Complex divide(const Complex& c) const {
        double denominator = c._real * c._real + c._imag * c._imag;
        if (fabs(denominator) < 1e-9) {
            cerr << "Error: Division by zero complex number." << endl;
            return Complex(0, 0);
        }
        double realPart = (_real * c._real + _imag * c._imag) / denominator;
        double imagPart = (_imag * c._real - _real * c._imag) / denominator;
        return Complex(realPart, imagPart);
    }

    double norm() const {
        return sqrt(_real * _real + _imag * _imag);
    }
    Complex conjugate() const {
        return Complex(_real, -_imag);
    }
    bool equals(const Complex& c) const {
        return (fabs(_real - c._real) < 1e-9) && 
               (fabs(_imag - c._imag) < 1e-9);
    }
    void print() const {
        if (fabs(_imag) < 1e-9) {
            cout << _real;
        } else if (fabs(_real) < 1e-9) {
            cout << _imag << "i";
        } else {
            cout << _real << ( _imag > 0 ? "+" : "" ) << _imag << "i";
        }
    }
    static bool compare(const Complex& a, const Complex& b) {
        if (a.norm() != b.norm()) {
            return a.norm() < b.norm();
        }
        return a._real < b._real;
    }

    // 重载相等运算符 ==
    bool operator==(const Complex& c) const {
        return equals(c);
    }

    // 重载输出运算符 <<
    friend ostream& operator<<(ostream& os, const Complex& c) {
        if (fabs(c._imag) < 1e-9) {
            os << c._real;
        } else if (fabs(c._real) < 1e-9) {
            os << c._imag << "i";
        } else {
            os << c._real << (c._imag > 0 ? "+" : "") << c._imag << "i";
        }
        return os;
    }
};

#endif  // COMPLEX_H