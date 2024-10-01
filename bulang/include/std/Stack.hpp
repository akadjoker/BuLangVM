

#pragma once
#include "Config.hpp"




template <typename V, size_t S>
class Stack
{
private:
    V values[S];
    size_t m_size;
    V *m_top;

public:
    Stack();
    ~Stack();


    void push(const V &value);

    bool empty();
  
    void pop();
    V &top();

  
    constexpr size_t size() ;
    void clear();
};




//************************************************************** */
//STACK
//************************************************************** */

#if defined(IMPLEMENTATION)



template <typename V, size_t S>
Stack<V, S>::Stack()
{
    m_size = 0;
    m_top = &values[0];
}

template <typename V, size_t S>
Stack<V, S>::~Stack()
{
}



template <typename V, size_t S>
bool Stack<V, S>::empty()
{
    return m_size == 0;
}


template <typename V, size_t S>
V &Stack<V, S>::top()
{
    return *m_top;
}


template <typename V, size_t S>
void Stack<V, S>::pop()
{
    DEBUG_BREAK_IF(m_size == 0);
    m_top--;
    m_size--;
}



template <typename V, size_t S>
void Stack<V, S>::push(const V &value)
{
    DEBUG_BREAK_IF(m_size >= S);
    *m_top = value;
    m_top++;
    m_size++;
}

template <typename V, size_t S>
void Stack<V, S>::clear()
{
    m_size = 0;
    m_top = &values[0];
}   

#endif