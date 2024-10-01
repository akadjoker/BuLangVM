

#pragma once
#include "Config.hpp"


template <typename V, size_t S>
class Queue
{
private:
    V values[S];
    size_t m_size;
    size_t m_front; 
    size_t m_back;   

public:
    Queue();
    ~Queue();

    void enqueue(const V &value);  
    void dequeue();                
    V &front();                    
    bool empty();                  
    constexpr size_t size();       
    void clear();                  
};

//************************************************************** */
// QUEUE  
//************************************************************** */
#if defined(IMPLEMENTATION)


template <typename V, size_t S>
Queue<V, S>::Queue() : m_size(0), m_front(0), m_back(0)
{
}

template <typename V, size_t S>
Queue<V, S>::~Queue()
{
}

template <typename V, size_t S>
bool Queue<V, S>::empty()
{
    return m_size == 0;
}

template <typename V, size_t S>
V &Queue<V, S>::front()
{
    DEBUG_BREAK_IF(m_size == 0);
    return values[m_front];
}

template <typename V, size_t S>
void Queue<V, S>::dequeue()
{
    DEBUG_BREAK_IF(m_size == 0);
    m_front = (m_front + 1) % S; 
    m_size--;
}

template <typename V, size_t S>
void Queue<V, S>::enqueue(const V &value)
{
    DEBUG_BREAK_IF(m_size >= S);
    values[m_back] = value;
    m_back = (m_back + 1) % S; 
    m_size++;
}

template <typename V, size_t S>
constexpr size_t Queue<V, S>::size()
{
    return m_size;
}

template <typename V, size_t S>
void Queue<V, S>::clear()
{
    m_size = 0;
    m_front = 0;
    m_back = 0;
}

#endif