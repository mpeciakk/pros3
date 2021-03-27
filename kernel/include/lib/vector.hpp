#ifndef __VECTOR_HPP
#define __VECTOR_HPP

#include <lib/string.hpp>
#include <types.hpp>

void memcpy(void* dest, void* src, u32 size);

template <class T> class Vector {
public:
    Vector() : Vector(0) { }

    Vector(int initialSize) : m_size(initialSize), m_capacity(initialSize) {
        dataPtr = new T[m_size];
    }

    ~Vector() {
        delete[] dataPtr;
    }

    inline int size() {
        return m_size;
    }

    inline int capacity() {
        return m_capacity;
    }

    void shrinkToFit() {
        if (m_size < m_capacity) {
            m_capacity = m_size;
            T* tmpPtr = new T[m_size];
            memcpy(tmpPtr, dataPtr, sizeof(T) * m_size);
            delete[] dataPtr;
            dataPtr = tmpPtr;
        }
    }

    void pushBack(const T& value) {
        m_size++;
        if (m_size > m_capacity) {
            m_capacity = m_size;
            T* tmpPtr = new T[m_size];
            memcpy(tmpPtr, dataPtr, sizeof(T) * (m_size - 1));
            delete[] dataPtr;
            dataPtr = tmpPtr;
        }
        dataPtr[m_size - 1] = value;
    }

    void popBack() {
        m_size--;
    }

    void pushFront(const T& value) {
        m_size++;
        m_capacity = m_size;
        T* tmpPtr = new T[m_size];
        tmpPtr[0] = value;
        memcpy(tmpPtr + 1, dataPtr, sizeof(T) * (m_size - 1));
        delete[] dataPtr;
        dataPtr = tmpPtr;
    }

    void popFront() {
        m_size--;
        m_capacity = m_size;
        T* tmpPtr = new T[m_size];
        memcpy(tmpPtr, dataPtr + 1, sizeof(T) * (m_size - 1));
        delete[] dataPtr;
        dataPtr = tmpPtr;
    }

    void remove(int index, int count) {
        if (index > m_size) {
            return;
        }

        if (count <= 0) {
            return;
        }

        T* tmpPtr = new T[m_size - count];

        memcpy(tmpPtr, dataPtr, index * sizeof(T));
        memcpy(tmpPtr + index, dataPtr + index + count, sizeof(T) * (m_size - index - count));

        delete[] dataPtr;
        dataPtr = tmpPtr;
        m_size -= count;
    }

    inline T& operator[](int index) {
        return dataPtr[index];
    }

private:
    int m_size;
    int m_capacity;
    T* dataPtr;
};

#endif
