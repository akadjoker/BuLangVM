#pragma once
#include <cstdint>
#include <cstring>
#include <utility>  

// Vector otimizado para tipos POD (sem constructor/destructor)
template <typename T>
class Vector
{
private:
    T *data_;
    size_t size_;
    size_t capacity_;
 

public:
    Vector()
        : data_(nullptr), size_(0), capacity_(0)
    {
        reserve(8);
    }



    explicit Vector(size_t initialCapacity)
        : data_(nullptr), size_(0), capacity_(0)
    {
        reserve(initialCapacity);
    }

    ~Vector()
    {
        destroy();
    }

    // Move
    Vector(Vector &&other) noexcept
        : data_(other.data_), size_(other.size_), capacity_(other.capacity_)
    {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    Vector &operator=(Vector &&other) noexcept
    {
        if (this != &other)
        {
            destroy();
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
     

            other.data_ = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }

    // Delete copy
    Vector(const Vector &) = delete;
    Vector &operator=(const Vector &) = delete;

    void destroy()
    {
        if (data_)
        {
            aFree(data_);
            data_ = nullptr;
            size_ = 0;
            capacity_ = 0;
        }
    }

    void reserve(size_t newCapacity)
    {
        if (newCapacity <= capacity_)
            return;

        // Aloca novo bloco
        T *newData = (T *)aAlloc(newCapacity * sizeof(T));

        // Copia dados antigos (POD - usa memcpy)
        if (data_)
        {
            std::memcpy(newData, data_, size_ * sizeof(T));
            aFree(data_);
        }

        data_ = newData;
        capacity_ = newCapacity;
    }

    void push(const T &value)
    {
        if (size_ >= capacity_)
        {
            //size_t newCap = capacity_ < 8 ? 8 : capacity_ * 2;
            size_t newCap=CalculateCapacityGrow(capacity_,size_+1);
            reserve(newCap);
        }

        data_[size_++] = value; // Simples assignment
    }

    template <typename... Args>
    void emplace(Args &&...args)
    {
        if (size_ >= capacity_)
        {
            //size_t newCap = capacity_ < 8 ? 8 : capacity_ * 2;
            size_t newCap=CalculateCapacityGrow(capacity_,size_+1);
            reserve(newCap);
        }

        // Para POD, isto funciona (aggregate initialization)
        data_[size_++] = T{std::forward<Args>(args)...};
    }
    void pop()
    {
        if (size_ > 0)
        {
            size_--;
        }
    }

    T &back()
    {
        return data_[size_ - 1];
    }

    const T &back() const
    {
        return data_[size_ - 1];
    }

    void clear()
    {
        size_ = 0; // Sem destructors!
    }

    void resize(size_t newSize)
    {
        if (newSize > capacity_)
        {
            reserve(newSize);
        }
        size_ = newSize;
    }

    // Accessors
    T &operator[](size_t i) { return data_[i]; }
    const T &operator[](size_t i) const { return data_[i]; }

    T *data() { return data_; }
    const T *data() const { return data_; }

    size_t size() const { return size_; }
    size_t capacity() const { return capacity_; }
    bool empty() const { return size_ == 0; }

    // Iterators
    T *begin() { return data_; }
    T *end() { return data_ + size_; }
    const T *begin() const { return data_; }
    const T *end() const { return data_ + size_; }
};