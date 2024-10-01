#pragma once

#include "Config.hpp"

template <typename T>
class SharedPtr
{
private:
    T *ptr;
    int *count;

public:
    explicit SharedPtr(T *p = nullptr)
        : ptr(p), count(p ? new int(1) : nullptr) {}

    explicit SharedPtr(std::nullptr_t)
        : ptr(nullptr), count(nullptr) {}

    SharedPtr(const SharedPtr &other)
        : ptr(other.ptr), count(other.count)
    {
        if (count)
        {
            (*count)++;
        }
    }

    template <typename U>
    SharedPtr(const SharedPtr<U> &other) : ptr(other.ptr), count(other.count)
    {
        if (count)
        {
            (*count)++;
        }
    }

    SharedPtr &operator=(SharedPtr other)
    {
        swap(*this, other);
        return *this;
    }

    SharedPtr &operator=(std::nullptr_t)
    {

        release();
        return *this;
    }

    ~SharedPtr()
    {
        release();
    }

    void release()
    {
        if (count && --(*count) == 0)
        {
            delete ptr;
            delete count;
        }
        ptr = nullptr;
        count = nullptr;
    }

    friend void swap(SharedPtr &first, SharedPtr &second) noexcept
    {
        T *tempPtr = first.ptr;
        int *tempCount = first.count;
        first.ptr = second.ptr;
        first.count = second.count;
        second.ptr = tempPtr;
        second.count = tempCount;
    }

    T &operator*() const { return *ptr; }
    T *operator->() const { return ptr; }
    T *get() const { return ptr; }
    int use_count() const { return count ? *count : 0; }
    bool unique() const { return use_count() == 1; }
    bool is_null() const { return ptr == nullptr; }

    explicit operator bool() const { return ptr != nullptr; }

    bool operator==(std::nullptr_t) const { return ptr == nullptr; }

    bool operator!=(std::nullptr_t) const { return ptr != nullptr; }

    template <typename U>
    friend class SharedPtr;
};

template <typename T>
class UniquePtr
{
private:
    T *ptr;

public:
    explicit UniquePtr(T *p = nullptr) : ptr(p) {}

    ~UniquePtr() { delete ptr; }

    UniquePtr(const UniquePtr &) = delete;
    UniquePtr &operator=(const UniquePtr &) = delete;
    UniquePtr(UniquePtr &&other) noexcept : ptr(other.ptr) { other.ptr = nullptr; }
    UniquePtr &operator=(UniquePtr &&other) noexcept
    {
        if (this != &other)
        {
            delete ptr;
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    template <typename U>
    UniquePtr(UniquePtr<U> &&other) noexcept : ptr(other.release()) {}

    template <typename U>
    UniquePtr &operator=(UniquePtr<U> &&other) noexcept
    {
        if (this != &other)
        {
            reset(other.release());
        }
        return *this;
    }

    UniquePtr &operator=(std::nullptr_t) noexcept
    {
        reset();
        return *this;
    }

    T &operator*() const { return *ptr; }
    T *operator->() const { return ptr; }
    T *get() const { return ptr; }

    T *release()
    {
        T *temp = ptr;
        ptr = nullptr;
        return temp;
    }

    void reset(T *p = nullptr)
    {
        if (ptr != p)
        {
            delete ptr;
            ptr = p;
        }
    }
    explicit operator bool() const { return ptr != nullptr; }

    bool is_null() const { return ptr == nullptr; }

    template <typename U>
    friend class UniquePtr;
};

template <typename T, typename... Args>
inline UniquePtr<T> Make_Unique(Args &&...args)
{
    return UniquePtr<T>(new T(std::forward<Args>(args)...));
}

template <typename T, typename... Args>
inline SharedPtr<T> Make_Shared(Args &&...args)
{
    return SharedPtr<T>(new T(std::forward<Args>(args)...));
}
