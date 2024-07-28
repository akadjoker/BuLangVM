

#pragma once

template <typename T>
class HashNode
{
public:
    char key[128]{'\0'};
    size_t len;
    T value;
    HashNode *next;

    HashNode(const char *k, const T &v) : value(v), next(nullptr)
    {
        len = strlen(k);
        memcpy(key, k, len + 1);
        key[len] = '\0';
    }
    ~HashNode()
    {
        next = nullptr;
    }
};

template <typename T>
class HashTable
{
private:
    HashNode<T> **table;

    u32 capacity;
    u32 size;
    HashNode<T> *m_Iter;
    u32 m_IterIndex;

    const float loadFactorThreshold = 0.75f;

    // u32 HashStr(const char *index) const
    // {
    //     u32 iIndex = 2166136261u;
    //     while (*index)
    //     {
    //         iIndex ^= *index;
    //         iIndex *= 16777619;
    //         index++;
    //     }
    //     iIndex = (iIndex >> 16) ^ iIndex;
    //     return iIndex & (capacity - 1); // capacity must be a power of 2
    // }

    u32 HashStr(const char *index) const
    {
        u32 iIndex = 2166136261u;
        while (*index)
        {
            iIndex ^= *index++;
            iIndex *= 16777619;

            if (*index)
            {
                iIndex ^= *index++;
                iIndex *= 16777619;
            }

            if (*index)
            {
                iIndex ^= *index++;
                iIndex *= 16777619;
            }

            if (*index)
            {
                iIndex ^= *index++;
                iIndex *= 16777619;
            }
        }
        iIndex = (iIndex >> 16) ^ iIndex;
        return iIndex & (capacity - 1); // capacity must be a power of 2
    }

    void resizeTable(u32 newCapacity)
    {
        u32 oldCapacity = capacity;
        capacity = newCapacity;
        size = 0; // size will be recalculated during rehashing

        HashNode<T> **newTable = new HashNode<T> *[newCapacity]();
        for (size_t i = 0; i < oldCapacity; ++i)
        {
            HashNode<T> *currentNode = table[i];
            while (currentNode)
            {
                u32 newIndex = HashStr(currentNode->key);
                HashNode<T> *temp = currentNode;
                currentNode = currentNode->next;

                temp->next = newTable[newIndex];
                newTable[newIndex] = temp;
                size++;
            }
        }
        delete[] table;
        table = newTable;
    }

public:
    HashTable(u32 Capacity = 16) : capacity(Capacity), size(0), m_Iter(nullptr), m_IterIndex(0)
    {
        table = new HashNode<T> *[capacity]();
    }

    ~HashTable()
    {
        clear();
        delete[] table;
    }

    T first()
    {
        if (size == 0)
            return T();
        T item = T();
        m_IterIndex = 0;
        m_Iter = nullptr;

        for (size_t i = 0; i < capacity; i++)
        {
            HashNode<T> *pCurrItem = table[i];
            while (pCurrItem)
            {
                if (!item)
                    item = pCurrItem->value;
                else
                {
                    m_Iter = pCurrItem;
                    m_IterIndex = i;
                    return item;
                }
                pCurrItem = pCurrItem->next;
            }
        }
        return item;
    }

    T next()
    {
        if (!m_Iter)
            return T();
        T item = m_Iter->value;
        if (m_Iter->next)
        {
            m_Iter = m_Iter->next;
            return item;
        }
        for (size_t i = m_IterIndex + 1; i < capacity; i++)
        {
            if (table[i])
            {
                m_Iter = table[i];
                m_IterIndex = i;
                return item;
            }
        }
        m_Iter = nullptr;
        m_IterIndex = 0;
        return item;
    }

    void insert(const char *key, const T &value)
    {
        if ((float)(size + 1) / capacity > loadFactorThreshold)
        {
            u32 newCapacity = CalculateCapacityGrow(capacity * 2, 8);
            resizeTable(newCapacity);
        }

        u32 index = HashStr(key);
        HashNode<T> *newNode = new HashNode<T>(key, value);
        newNode->next = table[index];
        table[index] = newNode;
        ++size;
    }

    bool find(const char *key, T &value) const
    {
        u32 index = HashStr(key);
        HashNode<T> *currentNode = table[index];
        while (currentNode)
        {
            if (matchString(key, currentNode->key, currentNode->len))
            // if (strcmp(currentNode->key, key) == 0)
            {
                value = currentNode->value;
                return true;
            }
            currentNode = currentNode->next;
        }
        return false;
    }

    T read(const char *key) 
    {
        u32 index = HashStr(key);
        HashNode<T> *currentNode = table[index];
        while (currentNode)
        {
            if (matchString(key, currentNode->key, currentNode->len))
            {
                return currentNode->value;
                
            }
            currentNode = currentNode->next;
        }
        DEBUG_BREAK_IF(true);
        return T();
    }

    bool contains(const char *key) const
    {
        u32 index = HashStr(key);
        HashNode<T> *currentNode = table[index];
        while (currentNode)
        {
            if (matchString(key, currentNode->key, currentNode->len))
          //   if (strcmp(currentNode->key, key) == 0)
            {
                return true;
            }
            currentNode = currentNode->next;
        }
        return false;
    }

    void erase(const char *key)
    {
        u32 index = HashStr(key);
        HashNode<T> *currentNode = table[index];
        HashNode<T> *prevNode = nullptr;
        while (currentNode)
        {
            if (matchString(key, currentNode->key, currentNode->len))
            // if (strcmp(currentNode->key, key) == 0)
            {
                if (prevNode)
                {
                    prevNode->next = currentNode->next;
                }
                else
                {
                    table[index] = currentNode->next;
                }
                delete currentNode;
                --size;
                return;
            }
            prevNode = currentNode;
            currentNode = currentNode->next;
        }
    }

    bool change(const char *key, T value)
    {
        u32 index = HashStr(key);
        HashNode<T> *currentNode = table[index];
        while (currentNode)
        {
            if (matchString(key, currentNode->key, currentNode->len))
            // if (strcmp(currentNode->key, key) == 0)
            {
                currentNode->value = value;
                return true;
            }
            currentNode = currentNode->next;
        }
        return false;
    }

    bool remove(const char *key, T &value)
    {
        u32 index = HashStr(key);
        HashNode<T> *currentNode = table[index];
        HashNode<T> *prevNode = nullptr;
        while (currentNode)
        {
            if (matchString(key, currentNode->key, currentNode->len))
            // if (strcmp(currentNode->key, key) == 0)
            {
                if (prevNode)
                {
                    prevNode->next = currentNode->next;
                }
                else
                {
                    table[index] = currentNode->next;
                }
                value = currentNode->value;
                delete currentNode;
                --size;
                return true;
            }
            prevNode = currentNode;
            currentNode = currentNode->next;
        }
        return false;
    }

    void clear()
    {
        for (size_t i = 0; i < capacity; ++i)
        {
            HashNode<T> *currentNode = table[i];
            while (currentNode)
            {
                HashNode<T> *nextNode = currentNode->next;
                delete currentNode;
                currentNode = nextNode;
            }
            table[i] = nullptr;
        }
        size = 0;
        m_Iter = nullptr;
        m_IterIndex = 0;
    }

    class Iterator
    {
    private:
        HashTable &hashTable;
        size_t bucketIndex;
        HashNode<T> *currentNode;

    public:
        Iterator(HashTable &ht, size_t index, HashNode<T> *node)
            : hashTable(ht), bucketIndex(index), currentNode(node) {}

        Iterator &operator++()
        {
            if (currentNode)
            {
                currentNode = currentNode->next;
            }
            while (!currentNode && ++bucketIndex < hashTable.capacity)
            {
                currentNode = hashTable.table[bucketIndex];
            }
            return *this;
        }

        bool operator!=(const Iterator &other) const
        {
            return bucketIndex != other.bucketIndex || currentNode != other.currentNode;
        }

        std::pair<const std::string &, T &> operator*()
        {
            return {currentNode->key, currentNode->value};
        }
    };

    Iterator begin()
    {
        size_t index = 0;
        HashNode<T> *node = nullptr;
        while (index < capacity && !(node = table[index]))
        {
            ++index;
        }
        return Iterator(*this, index, node);
    }

    Iterator end()
    {
        return Iterator(*this, capacity, nullptr);
    }
};