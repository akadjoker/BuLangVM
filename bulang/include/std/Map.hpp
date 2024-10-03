
#pragma once

#include "Config.hpp"
#include "String.hpp"
#include "Chars.hpp"

// container with hash map

template <typename K, typename V>
struct KeyItem
{
    K key;
    V value;
    KeyItem *next;
    KeyItem(const K &k, const V &v) : key(k), value(v), next(nullptr) {}
};



template <typename K, typename V>
class Map
{
private:
    KeyItem<K, V> **buckets;
    KeyItem<K, V> *m_inter;
    u32 m_size;
    u32 m_count;
    u32 m_power;
    int m_interIndex;
    float max_load_factor;

    KeyItem<K, V> *createNode(const K &key, const V &value);
    void deleteNode(KeyItem<K, V> *node);
    void rehash();
    void copyFrom(const Map &other);

public:
    Map(u32 size = 8, float load_factor = 0.75f);

    ~Map();

    // Copy Constructor
    Map(const Map &other);
    // Move Constructor
    Map(Map &&other) noexcept;
    // Move Assignment Operator
    Map &operator=(Map &&other) noexcept;
    // Copy Assignment Operator
    Map &operator=(const Map &other);

    bool operator==(const Map& other) const;

    KeyItem<K, V> *root(u32 index);
    KeyItem<K, V> *first();
    KeyItem<K, V> *next();

    void clear();

    bool contains(const K &key) const;

    bool find(const K &key, V &value) const;

    bool erase(const K &key);

    bool insert(const K &key, const V &value);
    bool assign(const K &key, const V &value);
    bool replace(const K &key, const V &value,  V old);
    constexpr u32 size() const { return m_size; }
    constexpr u32 count() const { return m_count; }
};

template <typename K, typename V>
Map<K, V>::Map(u32 size, float load_factor) : m_size(size), max_load_factor(load_factor)
{

    if (size < 2)
        size = 2;
    if (size > 0x7fffffff)
        size = 0x7fffffff;

    u32 new_size = 1;
    m_power = 0;
    while (new_size < size)
    {
        new_size = new_size << 1;
        m_power++;
    }
    m_size = new_size;

    buckets = new KeyItem<K, V> *[m_size];
    for (u32 i = 0; i < m_size; ++i)
    {
        buckets[i] = nullptr;
    }
    m_count = 0;
    m_inter = nullptr;
    m_interIndex = 0;
}

template <typename K, typename V>
Map<K, V>::~Map()
{
    clear();
}

template <typename K, typename V>
KeyItem<K, V> *Map<K, V>::root(u32 index)
{
    DEBUG_BREAK_IF(index >= m_size);
    return buckets[index];
}

template <typename K, typename V>
KeyItem<K, V> *Map<K, V>::first()
{
    if (buckets == nullptr || m_count == 0)
        return nullptr;

    KeyItem<K, V> *pReturnItem = nullptr;
    m_inter = nullptr;
    m_interIndex = 0;
    for (u32 i = 0; i < m_size; i++)
    {
        KeyItem<K, V> *pCurrItem = buckets[i];
        while (pCurrItem)
        {
            if (!pReturnItem)
                pReturnItem = pCurrItem->next;
            else
            {
                m_inter = pCurrItem;
                m_interIndex = i;
                return pReturnItem;
            }

            pCurrItem = pCurrItem->next;
        }
    }

    return pReturnItem;
}

template <typename K, typename V>
KeyItem<K, V> *Map<K, V>::next()
{

    if (!m_inter)
        return nullptr;

    KeyItem<K, V> *pReturnItem = m_inter;
    if (m_inter->next)
    {
        m_inter = m_inter->next;
        return pReturnItem;
    }

    for (u32 i = m_interIndex + 1; i < m_size; i++)
    {
        if (buckets[i])
        {
            m_inter = buckets[i];
            m_interIndex = i;
            return pReturnItem;
        }
    }

    m_inter = nullptr;
    m_interIndex = 0;
    return pReturnItem;
}

template <typename K, typename V>
void Map<K, V>::clear()
{
    if (buckets == nullptr)
        return;
    for (u32 i = 0; i < m_size; ++i)
    {
        KeyItem<K, V> *node = buckets[i];
        while (node)
        {
            KeyItem<K, V> *next = node->next;
            deleteNode(node);
            node = next;
        }
        buckets[i] = nullptr;
    }
    delete[] buckets;
    buckets = nullptr;
    m_count = 0;
}

inline u32 make_hash(int iIndex, u32 iPower, u32 iListSize)
{
    iIndex = (iIndex >> 16) ^ iIndex;
    iIndex = (iIndex >> iPower) ^ iIndex;
    return iIndex & (iListSize - 1);
}

inline u32 make_hash(u32 iIndex, u32 iPower, u32 iListSize)
{
    iIndex = (iIndex >> 16) ^ iIndex;
    iIndex = (iIndex >> iPower) ^ iIndex;
    return iIndex & (iListSize - 1);
}

inline u32 make_hash(const void *ptr, u32 iPower, u32 iListSize)
{
    unsigned long long ptr64 = (unsigned long long)ptr;
    unsigned int ptr32 = (unsigned int)ptr64;
    ptr32 = (ptr64 >> 32) ^ ptr32;
    ptr32 = (ptr32 >> 16) ^ ptr32;
    ptr32 = (ptr32 >> iPower) ^ ptr32;
    return ptr32 & (iListSize - 1); //  must be a power of 2
}

inline u32 make_hash(const char *str, u32 iPower, u32 iListSize)
{
    u32 index = 2166136261;
    while (*str)
    {
        index ^= *str;
        index *= 16777619;
        str++;
    }

    index = (index >> 16) ^ index;
    index = (index >> iPower) ^ index;
    return index & (iListSize - 1);
}

inline u32 make_hash(const String &string, u32 iPower, u32 iListSize)
{
    u32 index = 2166136261;
    const char *str = string.c_str();
    while (*str)
    {
        index ^= *str;
        index *= 16777619;
        str++;
    }

    index = (index >> 16) ^ index;
    index = (index >> iPower) ^ index;
    return index & (iListSize - 1);
}

inline u32 make_hash(const Chars &string, u32 iPower, u32 iListSize)
{
    u32 index = 2166136261;
    const char *str = string.c_str();
    while (*str)
    {
        index ^= *str;
        index *= 16777619;
        str++;
    }

    index = (index >> 16) ^ index;
    index = (index >> iPower) ^ index;
    return index & (iListSize - 1);
}

template <typename K, typename V>
bool Map<K, V>::contains(const K &key) const
{

    u32 index = make_hash(key, m_power, m_size);
    KeyItem<K, V> *current = buckets[index];
    while (current)
    {
        if (current->key == key)
        {
            return true;
        }
        current = current->next;
    }
    return false;
}

template <typename K, typename V>
bool Map<K, V>::find(const K &key, V &value) const
{
    u32 index = make_hash(key, m_power, m_size);
    KeyItem<K, V> *current = buckets[index];
    while (current)
    {
        if (current->key == key)
        {
            value = current->value;
            return true;
        }
        current = current->next;
    }

    return false;
}

template <typename K, typename V>
bool Map<K, V>::erase(const K &key)
{
    u32 index = make_hash(key, m_power, m_size);
    KeyItem<K, V> *current = buckets[index];
    KeyItem<K, V> *prev = nullptr;

    while (current)
    {
        if (current->key == key)
        {
            if (prev)
            {
                prev->next = current->next;
            }
            else
            {
                buckets[index] = current->next;
            }
            deleteNode(current);
            --m_count;
            return true;
        }
        prev = current;
        current = current->next;
    }

    return false;
}

template <typename K, typename V>
bool Map<K, V>::insert(const K &key, const V &value)
{
    if (static_cast<float>(m_count) / m_size > max_load_factor)
    {
        rehash();
    }

    u32 index = make_hash(key, m_power, m_size);
    KeyItem<K, V> *current = buckets[index];
    if (current)
    {
        while (current)
        {
            if (current->key == key)
            {
                return false;
            }
            current = current->next;
        }
    }

    KeyItem<K, V> *node = createNode(key, value);

    node->next = buckets[index];
    buckets[index] = node;
    m_count++;
    return true;
}
template <typename K, typename V>
bool Map<K, V>::assign(const K &key, const V &value)
{
    u32 index = make_hash(key, m_power, m_size);
    KeyItem<K, V> *current = buckets[index];
    while (current)
    {
        if (current->key == key)
        {
            current->value = value;
            return true;
        }
        current = current->next;
    }

    return false;
}

template <typename K, typename V>
bool Map<K, V>::replace(const K &key, const V &value, V old)
{
    u32 index = make_hash(key, m_power, m_size);
    KeyItem<K, V> *current = buckets[index];
    while (current)
    {
        if (current->key == key)
        {
            old = std::move(current->value);
            current->value = value;
            return true;
        }
        current = current->next;
    }
    m_count++;
    return false;
}

template <typename K, typename V>
KeyItem<K, V> *Map<K, V>::createNode(const K &key, const V &value)
{
    // void *p = m_arena.Allocate(sizeof(KeyItem<K, V>));
    // return new (p) KeyItem<K, V>(key, value);
    return new KeyItem<K, V>(key, value);
}

template <typename K, typename V>
void Map<K, V>::deleteNode(KeyItem<K, V> *node)
{
    delete node;

    // node->~KeyItem<K, V>();
    // m_arena.Free(node, sizeof(KeyItem<K, V>));
}

template <typename K, typename V>
void Map<K, V>::rehash()
{

    u32 new_size = 1;
    while (new_size < m_count)
    {
        new_size = new_size << 1;
    }
    new_size = new_size << 1;

    KeyItem<K, V> **new_buckets = new KeyItem<K, V> *[new_size];
    for (u32 i = 0; i < new_size; ++i)
    {
        new_buckets[i] = nullptr;
    }

    for (u32 i = 0; i < m_size; ++i)
    {
        KeyItem<K, V> *node = buckets[i];
        while (node)
        {
            KeyItem<K, V> *next = node->next;
            u32 index = make_hash(node->key, m_power, new_size);
            node->next = new_buckets[index];
            new_buckets[index] = node;
            node = next;
        }
    }

    delete[] buckets;
    buckets = new_buckets;
    m_size = new_size;
}

//**** */

template <typename K, typename V>
Map<K, V>::Map(const Map &other) : buckets(nullptr), m_size(0), m_count(0)
{
    copyFrom(other);
}

template <typename K, typename V>
Map<K, V> &Map<K, V>::operator=(const Map &other)
{
    if (this != &other)
    {
        clear();
        copyFrom(other);
    }
    return *this;
}

template <typename K, typename V>
Map<K, V>::Map(Map &&other) noexcept : buckets(other.buckets), m_size(other.m_size), m_count(other.m_count)
{
    other.buckets = nullptr;
    other.m_size = 0;
    other.m_count = 0;
}

template <typename K, typename V>
Map<K, V> &Map<K, V>::operator=(Map &&other) noexcept
{
    if (this != &other)
    {
        clear();
        buckets = other.buckets;
        m_size = other.m_size;
        m_count = other.m_count;

        other.buckets = nullptr;
        other.m_size = 0;
        other.m_count = 0;
    }
    return *this;
}

template <typename K, typename V>
bool Map<K, V>::operator==(const Map &other) const
{
    if (m_count != other.m_count || m_size != other.m_size)
    {
        return false;
    }
    for (u32 i = 0; i < m_size; ++i)
    {
        KeyItem<K, V> *current = buckets[i];
        KeyItem<K, V> *otherCurrent = other.buckets[i];
        while (current && otherCurrent)
        {
            if (current->key != otherCurrent->key || current->value != otherCurrent->value)
            {
                return false;
            }
            current = current->next;
            otherCurrent = otherCurrent->next;
        }
        if (current != nullptr || otherCurrent != nullptr)
        {
            return false;
        }
    }
    return true;
}



template <typename K, typename V>
void Map<K, V>::copyFrom(const Map &other)
{
    m_size = other.m_size;
    m_count = other.m_count;
    m_power = other.m_power;
    m_interIndex = other.m_interIndex;
    m_inter = other.m_inter;
    max_load_factor = other.max_load_factor;

    // Allocate new buckets
    buckets = new KeyItem<K, V> *[m_size];
    for (u32 i = 0; i < m_size; ++i)
    {
        buckets[i] = nullptr;
    }

    for (u32 i = 0; i < other.m_size; ++i)
    {
        KeyItem<K, V> *current = other.buckets[i];
        while (current)
        {
            insert(current->key, current->value);
            current = current->next;
        }
    }
}
