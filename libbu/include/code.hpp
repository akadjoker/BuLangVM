#pragma once

#include "config.hpp"
#include "array.hpp"
 
 

struct Value;


class Code
{
    size_t m_capacity;
    bool m_frozen; 
    public:
    Code(size_t capacity = 1024);
    
     void freeze() ;
    
    Code(const Code& other) = delete;  
    Code& operator=(const Code& other) = delete;
    
    void clear();
    void reserve(size_t capacity);

    void write(uint8 instruction, int line);
    void writeShort(uint16 value, int line);
    
    size_t capacity() const { return m_capacity; }

    uint8 operator[](size_t index);
    
    int addConstant(Value value);
    
    
    
    
    
    uint8 *code;
    int *lines;
    size_t count;
    Array constants;
};