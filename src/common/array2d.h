
#ifndef AOC_2023_ARRAY2D_H
#define AOC_2023_ARRAY2D_H

#include <vector>
#include <iostream>
#include <functional>

template<typename T>
class Array2D {
private:
    std::vector<T> m_data;
    std::size_t m_rows;
    std::size_t m_cols;

public:
    Array2D() : Array2D(0, 0) {
    }

    Array2D(std::size_t rows, std::size_t cols)
            : m_rows(rows), m_cols(cols) {
        m_data.resize(rows * cols);
    }

    bool isInBounds(int row, int col) {
        return row >= 0 && col >= 0 &&
                row < m_rows && col < m_cols;
    }

    T &operator[](std::size_t index) {
        if (index >= m_data.size()) {
            throw std::out_of_range("Array2D: Index out of bounds");
        }
        return m_data[index];
    }

    T &operator()(std::size_t row, std::size_t col) {
        if (row >= m_rows || col >= m_cols) {
            throw std::out_of_range("Array2D: Index out of bounds");
        }
        return m_data[row * m_cols + col];
    }

    const T &operator()(std::size_t row, std::size_t col) const {
        if (row >= m_rows || col >= m_cols) {
            throw std::out_of_range("Array2D: Index out of bounds");
        }
        return m_data[row * m_cols + col];
    }

    std::size_t rows() const { return m_rows; }

    std::size_t cols() const { return m_cols; }

    std::size_t size() const { return m_data.size(); }

    void resize(std::size_t rows, std::size_t cols) {
        m_rows = rows;
        m_cols = cols;
        m_data.resize(rows * cols);
    }

    typename std::vector<T>::iterator begin() {
        return m_data.begin();
    }

    typename std::vector<T>::iterator end() {
        return m_data.end();
    }

    typename std::vector<T>::const_iterator cbegin() const {
        return m_data.cbegin();
    }

    typename std::vector<T>::const_iterator cend() const {
        return m_data.cend();
    }

    Array2D<T> transpose() const {
        Array2D<T> transposed(m_cols, m_rows);
        for (std::size_t row = 0; row < m_rows; ++row) {
            for (std::size_t col = 0; col < m_cols; ++col) {
                transposed(col, row) = (*this)(row, col);
            }
        }
        return transposed;
    }

    class RowIterator {
    public:
        explicit RowIterator(T *ptr) : m_ptr(ptr) {}

        T &operator*() const { return *m_ptr; }

        T *operator->() { return m_ptr; }

        RowIterator &operator++() {
            ++m_ptr;
            return *this;
        }

        RowIterator operator++(int) {
            RowIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        RowIterator operator+(std::ptrdiff_t n) const {
            return RowIterator(m_ptr + n);
        }

        RowIterator operator-(std::ptrdiff_t n) const {
            return RowIterator(m_ptr - n);
        }

        friend bool operator==(const RowIterator &a, const RowIterator &b) {
            return a.m_ptr == b.m_ptr;
        }

        friend bool operator!=(const RowIterator &a, const RowIterator &b) {
            return a.m_ptr != b.m_ptr;
        }

        friend std::ptrdiff_t operator-(const RowIterator &lhs, const RowIterator &rhs) {
            return lhs.m_ptr - rhs.m_ptr;
        }

    private:
        T *m_ptr;
    };

    RowIterator rowBegin(std::size_t row) {
        return RowIterator(&m_data[row * m_cols]);
    }

    RowIterator rowEnd(std::size_t row) {
        return RowIterator(&m_data[row * m_cols + m_cols]);
    }

    class ReverseRowIterator {
    public:
        ReverseRowIterator(T *ptr) : m_ptr(ptr) {}

        T &operator*() const { return *m_ptr; }

        T *operator->() { return m_ptr; }

        ReverseRowIterator &operator++() {
            --m_ptr;
            return *this;
        }

        ReverseRowIterator operator++(int) {
            ReverseRowIterator tmp = *this;
            --m_ptr;
            return tmp;
        }

        ReverseRowIterator &operator--() {
            ++m_ptr;
            return *this;
        }

        ReverseRowIterator operator--(int) {
            ReverseRowIterator tmp = *this;
            ++m_ptr;
            return tmp;
        }

        ReverseRowIterator operator+(std::ptrdiff_t n) const {
            return ReverseRowIterator(m_ptr - n);
        }

        ReverseRowIterator operator-(std::ptrdiff_t n) const {
            return ReverseRowIterator(m_ptr + n);
        }

        // Difference between two iterators
        friend std::ptrdiff_t operator-(const ReverseRowIterator &lhs, const ReverseRowIterator &rhs) {
            return rhs.m_ptr - lhs.m_ptr;
        }

        friend bool operator==(const ReverseRowIterator &lhs, const ReverseRowIterator &rhs) {
            return lhs.m_ptr == rhs.m_ptr;
        }

        friend bool operator!=(const ReverseRowIterator &lhs, const ReverseRowIterator &rhs) {
            return lhs.m_ptr != rhs.m_ptr;
        }

    private:
        T *m_ptr;
    };

    ReverseRowIterator rowRBegin(std::size_t row) {
        return ReverseRowIterator(&m_data[row * m_cols + m_cols - 1]);
    }

    ReverseRowIterator rowREnd(std::size_t row) {
        return ReverseRowIterator(&m_data[row * m_cols - 1]);
    }

    class ColIterator {
    public:
        ColIterator(T *ptr, std::size_t stride) : m_ptr(ptr), m_stride(stride) {}

        T &operator*() const { return *m_ptr; }

        T *operator->() { return m_ptr; }

        ColIterator &operator++() {
            m_ptr += m_stride;
            return *this;
        }

        ColIterator operator++(int) {
            ColIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        ColIterator operator+(std::ptrdiff_t n) const {
            return ColIterator(m_ptr + n * m_stride, m_stride);
        }

        ColIterator operator-(std::ptrdiff_t n) const {
            return ColIterator(m_ptr - n * m_stride, m_stride);
        }

        friend bool operator==(const ColIterator &a, const ColIterator &b) {
            return a.m_ptr == b.m_ptr;
        }

        friend bool operator!=(const ColIterator &a, const ColIterator &b) {
            return a.m_ptr != b.m_ptr;
        }

        friend std::ptrdiff_t operator-(const ColIterator &lhs, const ColIterator &rhs) {
            return (lhs.m_ptr - rhs.m_ptr) / lhs.m_stride;
        }

    private:
        T *m_ptr;
        std::size_t m_stride;
    };

    ColIterator colBegin(std::size_t col) {
        return ColIterator(&m_data[col], m_cols);
    }

    ColIterator colEnd(std::size_t col) {
        return ColIterator(&m_data[m_rows * m_cols + col], m_cols);
    }

    class ReverseColIterator {
    public:
        ReverseColIterator(T *ptr, std::size_t stride) : m_ptr(ptr), m_stride(stride) {}

        T &operator*() const { return *m_ptr; }

        T *operator->() { return m_ptr; }

        ReverseColIterator &operator++() {
            m_ptr -= m_stride;
            return *this;
        }

        ReverseColIterator operator++(int) {
            ReverseColIterator tmp = *this;
            m_ptr -= m_stride;
            return tmp;
        }

        ReverseColIterator &operator--() {
            m_ptr += m_stride;
            return *this;
        }

        ReverseColIterator operator--(int) {
            ReverseColIterator tmp = *this;
            m_ptr += m_stride;
            return tmp;
        }

        ReverseColIterator operator+(std::ptrdiff_t n) const {
            return ReverseColIterator(m_ptr - n * m_stride, m_stride);
        }

        ReverseColIterator operator-(std::ptrdiff_t n) const {
            return ReverseColIterator(m_ptr + n * m_stride, m_stride);
        }

        friend std::ptrdiff_t operator-(const ReverseColIterator &lhs, const ReverseColIterator &rhs) {
            return (rhs.m_ptr - lhs.m_ptr) / lhs.m_stride;
        }

        friend bool operator==(const ReverseColIterator &lhs, const ReverseColIterator &rhs) {
            return lhs.m_ptr == rhs.m_ptr;
        }

        friend bool operator!=(const ReverseColIterator &lhs, const ReverseColIterator &rhs) {
            return lhs.m_ptr != rhs.m_ptr;
        }

    private:
        T *m_ptr;
        std::size_t m_stride;
    };

    ReverseColIterator colRBegin(std::size_t col) {
        return ReverseColIterator(&m_data[(m_rows - 1) * m_cols + col], m_cols);
    }

    ReverseColIterator colREnd(std::size_t col) {
        return ReverseColIterator(&m_data[col - m_cols], m_cols);
    }
};

#endif
