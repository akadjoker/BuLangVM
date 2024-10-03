

#pragma once
#include "Config.hpp"

template <typename V, size_t S>
class Array
{
private:
    V values[S];
    size_t m_count;
    size_t m_size;

public:
    Array() : m_count(0), m_size(S)
    {
    }

    ~Array() {}

    void push(const V &value)
    {
        DEBUG_BREAK_IF(m_count >= S);
        values[m_count] = value;
        m_count++;
    }

    void add(V value)
    {
        DEBUG_BREAK_IF(m_count >= S);

        values[m_count] = std::move(value);

        m_count++;
    }

    bool empty()
    {
        return m_count == 0;
    }

    void pop()
    {
        DEBUG_BREAK_IF(m_count == 0);
        m_count--;
    }

    V &back()
    {
        DEBUG_BREAK_IF(m_count == 0);
        return values[m_count - 1];
    }

    V &front()
    {
        DEBUG_BREAK_IF(m_count == 0);
        return values[0];
    }

    V &operator[](size_t index)
    {
        DEBUG_BREAK_IF(index >= S);
        return values[index];
    }

    const V &operator[](size_t index) const
    {
        DEBUG_BREAK_IF(index >= S);
        return values[index];
    }

    constexpr size_t size()
    {
        return m_count;
    }

    constexpr size_t capacity()
    {
        return S;
    }

    inline void clear()
    {
        m_count = 0;
    }
};
