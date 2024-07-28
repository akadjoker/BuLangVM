
#pragma once

template <typename T, typename Alloc = Allocator<T>>
class Stack
{
public:
    Stack();
    ~Stack();
    Stack(const Stack &other);
    Stack &operator=(const Stack &other);
    Stack(Stack &&other) noexcept;
    Stack &operator=(Stack &&other) noexcept;

    void push(const T &value);
    void push(T &&value);
    template <typename... Args>
    void emplace(Args &&...args);
    T pop();
    T &top();
    bool empty() const;
    size_t size() const;
    void clear();
    T &peek(size_t index) const;

    T back();

    T front();
   

    T &operator[](size_t index);
    const T &operator[](size_t index) const;

    void increment();

    void resize(size_t new_capacity);


private:
    T *data;
    size_t sz;
    size_t cap;
    Alloc alloc;

    
    void destroy_elements();
    void copy_from(const Stack &other);
};

template <typename T, typename Alloc>
Stack<T, Alloc>::Stack() : data(nullptr), sz(0), cap(0) {}

template <typename T, typename Alloc>
Stack<T, Alloc>::~Stack()
{
    destroy_elements();
    if (data)
    {
        alloc.deallocate(data, cap);
    }
}

template <typename T, typename Alloc>
Stack<T, Alloc>::Stack(const Stack &other) : data(nullptr), sz(0), cap(0)
{
    copy_from(other);
}

template <typename T, typename Alloc>
Stack<T, Alloc> &Stack<T, Alloc>::operator=(const Stack &other)
{
    if (this != &other)
    {
        destroy_elements();
        if (data)
        {
            alloc.deallocate(data, cap);
        }
        copy_from(other);
    }
    return *this;
}

template <typename T, typename Alloc>
Stack<T, Alloc>::Stack(Stack &&other) noexcept : data(other.data), sz(other.sz), cap(other.cap)
{
    other.data = nullptr;
    other.sz = 0;
    other.cap = 0;
}

template <typename T, typename Alloc>
Stack<T, Alloc> &Stack<T, Alloc>::operator=(Stack &&other) noexcept
{
    if (this != &other)
    {
        destroy_elements();
        if (data)
        {
            alloc.deallocate(data, cap);
        }

        data = other.data;
        sz = other.sz;
        cap = other.cap;

        other.data = nullptr;
        other.sz = 0;
        other.cap = 0;
    }
    return *this;
}

template <typename T, typename Alloc>
void Stack<T, Alloc>::push(const T &value)
{
    if (sz == cap)
    {
        resize(CalculateCapacityGrow(cap, sz + 1));
    }
    alloc.construct(&data[sz++], value);
}

template <typename T, typename Alloc>
void Stack<T, Alloc>::push(T &&value)
{
    if (sz == cap)
    {
        resize(CalculateCapacityGrow(cap, sz + 1));
    }
    alloc.construct(&data[sz++], std::move(value));
}

template <typename T, typename Alloc>
template <typename... Args>
void Stack<T, Alloc>::emplace(Args &&...args)
{
    if (sz == cap)
    {
        resize(CalculateCapacityGrow(cap, sz + 1));
    }
    alloc.construct(&data[sz++], std::forward<Args>(args)...);
}

template <typename T, typename Alloc>
T Stack<T, Alloc>::pop()
{
    DEBUG_BREAK_IF(sz == 0);
    T value = std::move(data[--sz]);
    alloc.destroy(&data[sz]);
    return value;
}

template <typename T, typename Alloc>
T &Stack<T, Alloc>::top()
{
    DEBUG_BREAK_IF(sz == 0);
    return data[sz - 1];
}

template <typename T, typename Alloc>
bool Stack<T, Alloc>::empty() const
{
    return sz == 0;
}

template <typename T, typename Alloc>
size_t Stack<T, Alloc>::size() const
{
    return sz;
}

template <typename T, typename Alloc>
inline void Stack<T, Alloc>::clear()
{
    destroy_elements();
    sz = 0;
}

template <typename T, typename Alloc>
inline T &Stack<T, Alloc>::peek(size_t index) const
{
    DEBUG_BREAK_IF(sz == 0);
    DEBUG_BREAK_IF(index >= sz);
    return data[sz - 1 - index];
}

template <typename T, typename Alloc>
inline T Stack<T, Alloc>::back()
{
    DEBUG_BREAK_IF(sz == 0);
    return data[sz - 1];
}

template <typename T, typename Alloc>
inline T Stack<T, Alloc>::front()
{
    DEBUG_BREAK_IF(sz == 0);
    return data[0];
}

template <typename T, typename Alloc>
inline T &Stack<T, Alloc>::operator[](size_t index)
{
    DEBUG_BREAK_IF(sz == 0);
    return data[index];
}

template <typename T, typename Alloc>
inline const T &Stack<T, Alloc>::operator[](size_t index) const
{
    DEBUG_BREAK_IF(sz == 0);
    return data[index];
}

template <typename T, typename Alloc>
inline void Stack<T, Alloc>::increment()
{
    ++sz;
}

template <typename T, typename Alloc>
void Stack<T, Alloc>::resize(size_t new_capacity)
{
    T *new_data = alloc.allocate(new_capacity);
    if (data && sz > 0)
    {
        if constexpr (std::is_trivially_copyable<T>::value)
        {
            std::memcpy(new_data, data, sz * sizeof(T));
        }
        else
        {
            for (size_t i = 0; i < sz; ++i)
            {
                alloc.construct(&new_data[i], std::move(data[i]));
                alloc.destroy(&data[i]);
            }
        }
    }
    if (data)
    {
        alloc.deallocate(data, cap);
    }
    data = new_data;
    cap = new_capacity;
}

template <typename T, typename Alloc>
void Stack<T, Alloc>::destroy_elements()
{
    for (size_t i = 0; i < sz; ++i)
    {
        alloc.destroy(&data[i]);
    }
}

template <typename T, typename Alloc>
void Stack<T, Alloc>::copy_from(const Stack &other)
{
    sz = other.sz;
    cap = other.cap;
    data = alloc.allocate(cap);
    if (other.data && sz > 0)
    {
        if constexpr (std::is_trivially_copyable<T>::value)
        {
            std::memcpy(data, other.data, sz * sizeof(T));
        }
        else
        {
            for (size_t i = 0; i < sz; ++i)
            {
                alloc.construct(&data[i], other.data[i]);
            }
        }
    }
}


