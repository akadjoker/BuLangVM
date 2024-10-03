#pragma once
#include "Config.hpp"

template <typename T, typename Alloc = Allocator<T>>
class Vector
{
public:
    Vector(size_t initial_capacity = 0);
    ~Vector();
    Vector(const Vector &other);
    Vector &operator=(const Vector &other);
    Vector(Vector &&other) noexcept;
    Vector &operator=(Vector &&other) noexcept;

    void push_back(const T &value);
    void push_back(T &&value);
    T pop_back();
    template <typename... Args>
    void emplace_back(Args &&...args);
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
    void push(const T &value);
    void pop();
    T &top();
    
    

private:
    T *data;
    Alloc alloc;
    size_t m_size;
    size_t m_capacity;

    void swap(Vector &other);
   
    void destroy_elements();
    void copy_from(const Vector &other);
};

template <typename T, typename Alloc>
Vector<T, Alloc>::Vector(size_t initial_capacity) : data(nullptr), m_size(0), m_capacity(initial_capacity)
{
    if (initial_capacity > 0)
    {

        data = alloc.allocate(initial_capacity);
  

        for (size_t i = 0; i < initial_capacity; i++)
        {
            alloc.construct(&data[i], std::move(T()));
        }
    
        m_capacity = initial_capacity;
        m_size = initial_capacity;

    }
}

template <typename T, typename Alloc>
Vector<T, Alloc>::~Vector()
{
    destroy_elements();
    if (data)
        alloc.deallocate(data);
}

template <typename T, typename Alloc>
Vector<T, Alloc>::Vector(const Vector &other) : data(nullptr), m_size(0), m_capacity(0)
{
    copy_from(other);
}

template <typename T, typename Alloc>
Vector<T, Alloc> &Vector<T, Alloc>::operator=(const Vector &other)
{
    if (this != &other)
    {
        destroy_elements();
        alloc.deallocate(data, m_capacity);
        copy_from(other);
    }
    return *this;
}

template <typename T, typename Alloc>
Vector<T, Alloc>::Vector(Vector &&other) noexcept : data(other.data), m_size(other.m_size), m_capacity(other.m_capacity)
{
    other.data = nullptr;
    other.m_size = 0;
    other.m_capacity = 0;
}

template <typename T, typename Alloc>
Vector<T, Alloc> &Vector<T, Alloc>::operator=(Vector &&other) noexcept
{
    if (this != &other)
    {
        destroy_elements();
        alloc.deallocate(data, m_capacity);

        data = other.data;
        m_size = other.m_size;
        m_capacity = other.m_capacity;

        other.data = nullptr;
        other.m_size = 0;
        other.m_capacity = 0;
    }
    return *this;
}

template <typename T, typename Alloc>
void Vector<T, Alloc>::push_back(const T &value)
{
    if (m_size == m_capacity)
    {
        reserve(CalculateCapacityGrow(m_capacity, m_size + 1));
    }

    alloc.construct(&data[m_size], value);
    
    ++m_size;
}

template <typename T, typename Alloc>
void Vector<T, Alloc>::push_back(T &&value)
{
    if (m_size == m_capacity)
    {
        reserve(CalculateCapacityGrow(m_capacity, m_size + 1));
    }

    alloc.construct(&data[m_size], std::move(value));

    ++m_size;
}

template <typename T, typename Alloc>
T Vector<T, Alloc>::pop_back()
{
    DEBUG_BREAK_IF(m_size == 0);

    T value;

    value = std::move(data[m_size]);
    alloc.destroy(&data[m_size]);
    m_size--;

    return value;
}

template <typename T, typename Alloc>
template <typename... Args>
void Vector<T, Alloc>::emplace_back(Args &&...args)
{
    if (m_size == m_capacity)
    {
        reserve(CalculateCapacityGrow(m_capacity, m_size + 1));
    }
    alloc.construct(&data[m_size++], std::forward<Args>(args)...);
}

template <typename T, typename Alloc>
T &Vector<T, Alloc>::operator[](size_t index)
{
    DEBUG_BREAK_IF(index >= m_size);
    return data[index];
}

template <typename T, typename Alloc>
inline const T &Vector<T, Alloc>::operator[](size_t index) const
{
    DEBUG_BREAK_IF(index >= m_size);
    return data[index];
}

template <typename T, typename Alloc>
size_t Vector<T, Alloc>::size() const
{
    return m_size;
}

template <typename T, typename Alloc>
size_t Vector<T, Alloc>::capacity() const
{
    return m_capacity;
}

template <typename T, typename Alloc>
T &Vector<T, Alloc>::back()
{
    DEBUG_BREAK_IF(m_size == 0);
    return data[m_size - 1];
}

template <typename T, typename Alloc>
bool Vector<T, Alloc>::empty() const
{
    return m_size == 0;
}

template <typename T, typename Alloc>
void Vector<T, Alloc>::destroy_elements()
{

    for (size_t i = 0; i < m_size; ++i)
    {
        alloc.destroy(&data[i]);
    }
}

template <typename T, typename Alloc>
void Vector<T, Alloc>::resize(size_t new_capacity)
{
    if (new_capacity <= m_capacity)
        return;

    T *new_data = alloc.allocate(new_capacity);
    if (data && m_size > 0)
    {

        for (size_t i = 0; i < m_size; ++i)
        {
            alloc.construct(&new_data[i], std::move(data[i]));
            alloc.destroy(&data[i]);
        }
        for (size_t i = m_size; i < new_capacity; i++)
        {
            alloc.construct(&new_data[i], T());
        }
    }
    else
    {

        for (size_t i = 0; i < new_capacity; i++)
        {
            alloc.construct(&new_data[i], std::move(T()));
        }
    }
    alloc.deallocate(data, m_capacity);
    data = new_data;
    m_capacity = new_capacity;
    m_size = new_capacity;
}

template <typename T, typename Alloc>
void Vector<T, Alloc>::reserve(size_t new_capacity)
{
    if (new_capacity > m_capacity)
    {
        T *new_data = alloc.allocate(new_capacity);
        if (data && m_size > 0)
        {

            for (size_t i = 0; i < m_size; ++i)
            {
                alloc.construct(&new_data[i], std::move(data[i]));
                alloc.destroy(&data[i]);
            }
        }
        alloc.deallocate(data);
        data = new_data;
        m_capacity = new_capacity;
    }
}

template <typename T, typename Alloc>
inline void Vector<T, Alloc>::clear()
{
    destroy_elements();
    m_size = 0;
}

template <typename T, typename Alloc>
inline void Vector<T, Alloc>::push(const T &value)
{
    push_back(value);
}

template <typename T, typename Alloc>
inline void Vector<T, Alloc>::pop()
{
    pop_back();
}

template <typename T, typename Alloc>
inline T &Vector<T, Alloc>::top()
{
    DEBUG_BREAK_IF(m_size == 0);
    return data[m_size - 1];
}

template <typename T, typename Alloc>
void Vector<T, Alloc>::swap(Vector &other)
{
    std::swap(data, other.data);
    std::swap(m_size, other.m_size);
    std::swap(m_capacity, other.m_capacity);
}

template <typename T, typename Alloc>
void Vector<T, Alloc>::copy_from(const Vector &other)
{
    m_size = other.m_size;
    m_capacity = other.m_capacity;
    data = alloc.allocate(m_capacity);
    if (other.data && m_size > 0)
    {

        for (size_t i = 0; i < m_size; ++i)
        {
            alloc.construct(&data[i], other.data[i]);
        }
    }
}
