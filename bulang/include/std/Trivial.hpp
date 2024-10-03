#pragma once
#include "Config.hpp"

template <typename T, typename Alloc = Allocator<T>>
class Trivial
{
public:
    Trivial(size_t initial_capacity = 0);
    ~Trivial();
    Trivial(const Trivial &other);
    Trivial &operator=(const Trivial &other);
    Trivial(Trivial &&other) noexcept;
    Trivial &operator=(Trivial &&other) noexcept;

    void push_back(const T &value);
    void push_back(T &&value);
    T pop_back();

    T &operator[](size_t index);
    const T &operator[](size_t index) const;
    size_t size() const;
    size_t capacity() const;
    void reserve(size_t new_capacity);
    T &back();
    bool empty() const;
    void resize(size_t new_capacity);
    void clear();

//stack
    T& top();
    void pop();
    void push(const T &value);
    void push(T &&value);

    T* data() { return m_data; }
    const T* data() const { return m_data; }

private:
    T *m_data;
    size_t m_size;
    size_t m_capacity;
    Alloc alloc;

    void copy_from(const Trivial &other);
};

template <typename T, typename Alloc>
Trivial<T, Alloc>::Trivial(size_t initial_capacity) : m_data(nullptr), m_size(0), m_capacity(initial_capacity)
{
    if (initial_capacity > 0)
    {
        m_data = alloc.allocate(initial_capacity);
    }
}

template <typename T, typename Alloc>
Trivial<T, Alloc>::~Trivial()
{

    if (m_data)
        alloc.deallocate(m_data, m_capacity);
}

template <typename T, typename Alloc>
Trivial<T, Alloc>::Trivial(const Trivial &other) : m_data(nullptr), m_size(0), m_capacity(0)
{
    copy_from(other);
}

template <typename T, typename Alloc>
Trivial<T, Alloc> &Trivial<T, Alloc>::operator=(const Trivial &other)
{
    if (this != &other)
    {

        alloc.deallocate(m_data, m_capacity);
        copy_from(other);
    }
    return *this;
}

template <typename T, typename Alloc>
Trivial<T, Alloc>::Trivial(Trivial &&other) noexcept : m_data(other.m_data), m_size(other.m_size), m_capacity(other.m_capacity)
{
    other.m_data = nullptr;
    other.m_size = 0;
    other.m_capacity = 0;
}

template <typename T, typename Alloc>
Trivial<T, Alloc> &Trivial<T, Alloc>::operator=(Trivial &&other) noexcept
{
    if (this != &other)
    {

        alloc.deallocate(m_data, m_capacity);

        m_data = other.m_data;
        m_size = other.m_size;
        m_capacity = other.m_capacity;

        other.m_data = nullptr;
        other.m_size = 0;
        other.m_capacity = 0;
    }
    return *this;
}

template <typename T, typename Alloc>
void Trivial<T, Alloc>::push_back(const T &value)
{
    if (m_size == m_capacity)
    {
        reserve(CalculateCapacityGrow(m_capacity, m_size + 1));
    }
    std::memcpy(&m_data[m_size], &value, sizeof(T));
    ++m_size;
}

template <typename T, typename Alloc>
void Trivial<T, Alloc>::push_back(T &&value)
{
    if (m_size == m_capacity)
    {
        reserve(CalculateCapacityGrow(m_capacity, m_size + 1));
    }
    std::memcpy(&m_data[m_size], &value, sizeof(T));
    ++m_size;
}

template <typename T, typename Alloc>
T Trivial<T, Alloc>::pop_back()
{
    DEBUG_BREAK_IF(m_size == 0);

    return m_data[--m_size];
}

template <typename T, typename Alloc>
T &Trivial<T, Alloc>::operator[](size_t index)
{
    DEBUG_BREAK_IF(index >= m_size);
    return m_data[index];
}

template <typename T, typename Alloc>
inline const T &Trivial<T, Alloc>::operator[](size_t index) const
{
    DEBUG_BREAK_IF(index >= m_size);
    return m_data[index];
}

template <typename T, typename Alloc>
size_t Trivial<T, Alloc>::size() const
{
    return m_size;
}

template <typename T, typename Alloc>
size_t Trivial<T, Alloc>::capacity() const
{
    return m_capacity;
}

template <typename T, typename Alloc>
T &Trivial<T, Alloc>::back()
{
    DEBUG_BREAK_IF(m_size == 0);
    return m_data[m_size - 1];
}

template <typename T, typename Alloc>
bool Trivial<T, Alloc>::empty() const
{
    return m_size == 0;
}

template <typename T, typename Alloc>
void Trivial<T, Alloc>::resize(size_t new_capacity)
{
    if (new_capacity <= m_capacity)
        return;

    T *new_data = alloc.allocate(new_capacity);
    if (m_data && m_size > 0)
    {

        std::memcpy(new_data, m_data, m_size * sizeof(T));
        std::memset(new_data + m_size, 0, (new_capacity - m_size) * sizeof(T));
    }
    else
    {

        std::memset(new_data, 0, new_capacity * sizeof(T));
    }
    alloc.deallocate(m_data, m_capacity);
    m_data = new_data;
    m_capacity = new_capacity;
    m_size = new_capacity;
}

template <typename T, typename Alloc>
void Trivial<T, Alloc>::reserve(size_t new_capacity)
{
    if (new_capacity > m_capacity)
    {
        T *new_data = alloc.allocate(new_capacity);
        if (m_data && m_size > 0)
        {

            std::memcpy(new_data, m_data, m_size * sizeof(T));
        }
        alloc.deallocate(m_data, m_capacity);
        m_data = new_data;
        m_capacity = new_capacity;
    }
}

template <typename T, typename Alloc>
inline void Trivial<T, Alloc>::clear()
{
    m_size = 0;
}

template <typename T, typename Alloc>
inline T &Trivial<T, Alloc>::top()
{
    DEBUG_BREAK_IF(m_size == 0);
    return m_data[m_size - 1];
}

template <typename T, typename Alloc>
inline void Trivial<T, Alloc>::pop()
{
    DEBUG_BREAK_IF(m_size == 0);
    --m_size;
}

template <typename T, typename Alloc>
inline void Trivial<T, Alloc>::push(const T &value)
{
    if (m_size == m_capacity)
    {
        reserve(CalculateCapacityGrow(m_capacity, m_size + 1));
    }
    std::memcpy(&m_data[m_size], &value, sizeof(T));
    ++m_size;
}

template <typename T, typename Alloc>
inline void Trivial<T, Alloc>::push(T &&value)
{
    if (m_size == m_capacity)
    {
        reserve(CalculateCapacityGrow(m_capacity, m_size + 1));
    }
    std::memcpy(&m_data[m_size], &value, sizeof(T));
    ++m_size;
}

template <typename T, typename Alloc>
void Trivial<T, Alloc>::copy_from(const Trivial &other)
{
    m_size = other.m_size;
    m_capacity = other.m_capacity;
    m_data = alloc.allocate(m_capacity);
    if (other.m_data && m_size > 0)
    {

        std::memcpy(m_data, other.m_data, m_size * sizeof(T));
    }
}
 