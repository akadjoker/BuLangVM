

template <typename T, typename Alloc = Allocator<T>>
class Queue
{
public:
    Queue();
    ~Queue();
    Queue(const Queue &other);
    Queue &operator=(const Queue &other);
    Queue(Queue &&other) noexcept;
    Queue &operator=(Queue &&other) noexcept;

    template <typename... Args>
    void emplace(Args &&...args);

    void push(const T &value);
    void push(T &&value);
    T pop();
    T &front();
    T &back();
    bool empty() const;
    size_t size() const;

private:
    T *data;
    size_t sz;
    size_t cap;
    size_t head;
    size_t tail;
    Alloc alloc;

    void resize(size_t new_capacity);
    void destroy_elements();
    void copy_from(const Queue &other);
};
template <typename T, typename Alloc>
Queue<T, Alloc>::Queue() : data(nullptr), sz(0), cap(0), head(0), tail(0) {}

template <typename T, typename Alloc>
Queue<T, Alloc>::~Queue()
{
    destroy_elements();
    if (data)
    {
        alloc.deallocate(data, cap);
    }
}

template <typename T, typename Alloc>
Queue<T, Alloc>::Queue(const Queue &other) : data(nullptr), sz(0), cap(0), head(0), tail(0)
{
    copy_from(other);
}

template <typename T, typename Alloc>
Queue<T, Alloc> &Queue<T, Alloc>::operator=(const Queue &other)
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
Queue<T, Alloc>::Queue(Queue &&other) noexcept
    : data(other.data), sz(other.sz), cap(other.cap), head(other.head), tail(other.tail)
{
    other.data = nullptr;
    other.sz = 0;
    other.cap = 0;
    other.head = 0;
    other.tail = 0;
}

template <typename T, typename Alloc>
Queue<T, Alloc> &Queue<T, Alloc>::operator=(Queue &&other) noexcept
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
        head = other.head;
        tail = other.tail;

        other.data = nullptr;
        other.sz = 0;
        other.cap = 0;
        other.head = 0;
        other.tail = 0;
    }
    return *this;
}

template <typename T, typename Alloc>
void Queue<T, Alloc>::push(const T &value)
{
    if (sz == cap)
    {
        resize(CalculateCapacityGrow(cap, sz + 1));
    }
    alloc.construct(&data[tail], value);
    tail = (tail + 1) % cap;
    ++sz;
}

template <typename T, typename Alloc>
void Queue<T, Alloc>::push(T &&value)
{
    if (sz == cap)
    {
        resize(CalculateCapacityGrow(cap, sz + 1));
    }
    alloc.construct(&data[tail], std::move(value));
    tail = (tail + 1) % cap;
    ++sz;
}

template <typename T, typename Alloc>
template <typename... Args>
void Queue<T, Alloc>::emplace(Args &&...args)
{
    if (sz == cap)
    {
        resize(CalculateCapacityGrow(cap, sz + 1));
    }
    alloc.construct(&data[tail], std::forward<Args>(args)...);
    tail = (tail + 1) % cap;
    ++sz;
}

template <typename T, typename Alloc>
T Queue<T, Alloc>::pop()
{
    if (sz == 0)
    {
        throw std::out_of_range("Queue is empty");
    }
    T value = std::move(data[head]);
    alloc.destroy(&data[head]);
    head = (head + 1) % cap;
    --sz;
    return value;
}

template <typename T, typename Alloc>
T &Queue<T, Alloc>::front()
{
    if (sz == 0)
    {
        throw std::out_of_range("Queue is empty");
    }
    return data[head];
}

template <typename T, typename Alloc>
T &Queue<T, Alloc>::back()
{
    if (sz == 0)
    {
        throw std::out_of_range("Queue is empty");
    }
    return data[(tail - 1 + cap) % cap];
}

template <typename T, typename Alloc>
bool Queue<T, Alloc>::empty() const
{
    return sz == 0;
}

template <typename T, typename Alloc>
size_t Queue<T, Alloc>::size() const
{
    return sz;
}

template <typename T, typename Alloc>
void Queue<T, Alloc>::resize(size_t new_capacity)
{
    T *new_data = alloc.allocate(new_capacity);
    if (data && sz > 0)
    {
        if (head < tail)
        {
            // Copy the elements in a single step
            if constexpr (std::is_trivially_copyable<T>::value)
            {
                std::memcpy(new_data, data + head, sz * sizeof(T));
            }
            else
            {
                for (size_t i = 0; i < sz; ++i)
                {
                    alloc.construct(&new_data[i], std::move(data[(head + i) % cap]));
                    alloc.destroy(&data[(head + i) % cap]);
                }
            }
        }
        else
        {
            // Copy the elements in two steps
            size_t front_size = cap - head;
            if constexpr (std::is_trivially_copyable<T>::value)
            {
                std::memcpy(new_data, data + head, front_size * sizeof(T));
                std::memcpy(new_data + front_size, data, tail * sizeof(T));
            }
            else
            {
                for (size_t i = 0; i < front_size; ++i)
                {
                    alloc.construct(&new_data[i], std::move(data[head + i]));
                    alloc.destroy(&data[head + i]);
                }
                for (size_t i = 0; i < tail; ++i)
                {
                    alloc.construct(&new_data[front_size + i], std::move(data[i]));
                    alloc.destroy(&data[i]);
                }
            }
        }
    }
    if (data)
    {
        alloc.deallocate(data, cap);
    }
    data = new_data;
    head = 0;
    tail = sz;
    cap = new_capacity;
}

template <typename T, typename Alloc>
void Queue<T, Alloc>::destroy_elements()
{
    if (data)
    {
        for (size_t i = 0; i < sz; ++i)
        {
            alloc.destroy(&data[(head + i) % cap]);
        }
    }
}

template <typename T, typename Alloc>
void Queue<T, Alloc>::copy_from(const Queue &other)
{
    sz = other.sz;
    cap = other.cap;
    head = 0;
    tail = sz;
    data = alloc.allocate(cap);
    if (other.data && sz > 0)
    {
        if (other.head < other.tail)
        {
            std::memcpy(data, other.data + other.head, sz * sizeof(T));
        }
        else
        {
            size_t front_size = other.cap - other.head;
            std::memcpy(data, other.data + other.head, front_size * sizeof(T));
            std::memcpy(data + front_size, other.data, other.tail * sizeof(T));
        }
    }
}
