#pragma once

#include <vector>

template <class T>
class Matrix {
    T *data;
    int r, c;

public:
    Matrix();
    Matrix(int r, int c);

    Matrix(const Matrix &m);
    Matrix(Matrix &&m);

    Matrix(const std::vector<std::vector<T>> &m);

    ~Matrix();

    Matrix &operator=(const Matrix &m);
    Matrix &operator=(Matrix &&m);

    int rows() const;
    int columns() const;

    T at(int i, int j) const;

    T *operator[](int i);
};

template <class T>
Matrix<T>::Matrix()
    : data(0), r(0), c(0) {
}

template <class T>
Matrix<T>::Matrix(int r, int c)
    : r(r), c(c) {
    data = new T[r * c];
}

template <class T>
Matrix<T>::Matrix(const Matrix &m)
    : r(m.r), c(m.c) {
    data = new T[r * c];

    for (int i = 0; i < r * c; i++)
        data[i] = m.data[i];
}

template <class T>
Matrix<T>::Matrix(const std::vector<std::vector<T>> &m) {
    r = m[0].size();
    c = m.size();

    data = new T[r * c];

    for (int i = 0; i < c; i++)
        for (int j = 0; j < r; j++)
            (*this)[i][j] = m[i][j];
}

template <class T>
Matrix<T>::Matrix(Matrix<T> &&m)
    : data(m.data), r(m.r), c(m.c) {
    m.data = 0;
    m.r = 0;
    m.c = 0;
}

template <class T>
Matrix<T>::~Matrix() {
    delete[] data;
}

template <class T>
Matrix<T> &Matrix<T>::operator=(const Matrix<T> &m) {
    r = m.r;
    c = m.c;

    delete[] data;

    data = new T[r * c];

    for (int i = 0; i < r * c; i++)
        data[i] = m.data[i];

    return *this;
}

template <class T>
Matrix<T> &Matrix<T>::operator=(Matrix<T> &&m) {
    r = m.r;
    c = m.c;

    delete[] data;

    data = m.data;

    m.data = 0;
    m.r = 0;
    m.c = 0;

    return *this;
}

template <class T>
int Matrix<T>::rows() const {
    return r;
}

template <class T>
int Matrix<T>::columns() const {
    return c;
}

template <class T>
T Matrix<T>::at(int i, int j) const {
    return data[i * r + j];
}

template <class T>
T *Matrix<T>::operator[](int i) {
    return data + i * r;
}
