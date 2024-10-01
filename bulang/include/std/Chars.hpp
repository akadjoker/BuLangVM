

#pragma once
#include "Config.hpp"

class Chars
{
private:
    static constexpr size_t SSO_BUFFER_SIZE = 15; // Tamanho do buffer SSO para strings pequenas

    size_t m_length{0};
    size_t m_capacity{0};
    union
    {
        char sso_data[SSO_BUFFER_SIZE + 1];
        char *m_data;
    };
    bool is_heap{false};

    void copy_data(const char *src, size_t len);
    void expand_capacity(size_t new_capacity);

public:
    Chars();

    Chars(const char *str);

    Chars(const char *str, size_t length);

    Chars(const Chars &other);

    Chars(Chars &&other) noexcept;
    Chars(char c);

    explicit Chars(int number);
    explicit Chars(float number);
    explicit Chars(double number);
    ~Chars();

    Chars &operator=(const Chars &str);
    Chars &operator+=(const char *rhs);
    Chars &operator+=(char c);
    Chars &operator+=(const Chars &rhs);
    Chars &operator=(Chars &&str) noexcept;
    bool operator==(const Chars &rhs) const;
    bool operator!=(const Chars &rhs) const;
     char &operator[](u32 index);
    const char &operator[](u32 index) const;

    size_t length() const { return m_length; }
    size_t size() const { return m_length; }
    size_t capacity() const { return m_capacity; }
    bool empty() const { return m_length == 0; }
    const char *c_str() const
    {
        return (is_heap ? m_data : sso_data);
    }

    float ToFloat() const;
    double ToDouble() const;
    int ToInt() const;
};

#if defined(IMPLEMENTATION)

void Chars::copy_data(const char *src, size_t len)
{
    if (!src)
    {
        m_length = 0;
        sso_data[0] = '\0';
        is_heap = false;
        m_capacity = 1;
        return;
    }
    if (len > SSO_BUFFER_SIZE)
    {
        if (is_heap)
            delete[] m_data;
        m_data = new char[len + 1];
        std::memcpy(m_data, src, len);
        m_data[len] = '\0';
        is_heap = true;
    }
    else
    {
        if (is_heap && m_data)
            delete[] m_data;
        std::memcpy(sso_data, src, len);
        sso_data[len] = '\0';
        is_heap = false;
    }
    m_length = len;
    m_capacity = len + 1;
}

void Chars::expand_capacity(size_t new_capacity)
{
    char *new_data = new char[new_capacity];
    std::memcpy(new_data, c_str(), m_length + 1);
    if (is_heap)
    {
        delete[] m_data;
    }
    m_data = new_data;
    m_capacity = new_capacity;
    is_heap = true;
}

Chars::Chars()
{
    copy_data(nullptr, 0);
}

Chars::Chars(const char *str)
{
    if (str)
    {
        copy_data(str, strlen(str));
    }
    else
    {
        copy_data(nullptr, 0);
    }
}

Chars::Chars(const char *str, size_t length)
{
    copy_data(str, length);
}

Chars::Chars(const Chars &other)
{
    copy_data(other.c_str(), other.m_length);
}

Chars::Chars(Chars &&other) noexcept
{
    // Move constructor
    if (other.is_heap)
    {
        m_data = other.m_data;
        is_heap = true;
    }
    else
    {
        std::memcpy(sso_data, other.sso_data, SSO_BUFFER_SIZE + 1);
        is_heap = false;
    }
    m_length = other.m_length;
    m_capacity = other.m_capacity;
    other.m_length = 0;
    other.m_capacity = 1;
    other.is_heap = false;
    other.m_data = nullptr;
    other.sso_data[0] = '\0';
}

Chars::Chars(char c)
{
    copy_data(&c, 1);
}

 Chars::Chars(int number) : Chars()
{
    char tempBuffer[128];
    sprintf(tempBuffer, "%d", number);
    *this = tempBuffer;
}

 Chars::Chars(float number) : Chars()
{
    char tempBuffer[128];
    sprintf(tempBuffer, "%f", number);
    *this = tempBuffer;
}

 Chars::Chars(double number) : Chars()
{
    char tempBuffer[128];
    sprintf(tempBuffer, "%f", number);
    *this = tempBuffer;
}

Chars::~Chars()
{
    if (is_heap)
    {
        delete[] m_data;
    }
}

// Operador  atribuição de cópia
Chars &Chars::operator=(const Chars &str)
{
    if (this == &str)
        return *this;

    copy_data(str.c_str(), str.m_length);
    return *this;
}
Chars &Chars::operator+=(const char *rhs)
{
    size_t rhs_length = strlen(rhs);
    size_t new_length = m_length + rhs_length;

    if (new_length > SSO_BUFFER_SIZE)
    {
        if (!is_heap || new_length + 1 > m_capacity)
        {
            expand_capacity(new_length + 1);
        }
        std::memcpy(m_data + m_length, rhs, rhs_length + 1);
    }
    else
    {
        std::memcpy(sso_data + m_length, rhs, rhs_length + 1);
    }

    m_length = new_length;
    return *this;
}
Chars &Chars::operator+=(char c)
{
    char temp[2] = {c, '\0'};
    return *this += temp;
}
Chars &Chars::operator+=(const Chars &rhs)
{
    return *this += rhs.c_str();
}

// Operador  atribuição de movimento
Chars &Chars::operator=(Chars &&str) noexcept
{
    if (this == &str)
        return *this;

    if (is_heap)
        delete[] m_data;

    if (str.is_heap)
    {
        m_data = str.m_data;
        is_heap = true;
    }
    else
    {
        std::memcpy(sso_data, str.sso_data, SSO_BUFFER_SIZE + 1);
        is_heap = false;
    }

    m_length = str.m_length;
    m_capacity = str.m_capacity;

    str.m_length = 0;
    str.m_capacity = 1;
    str.is_heap = false;
    str.m_data = nullptr;
    str.sso_data[0] = '\0';

    return *this;
}

bool Chars::operator==(const Chars &rhs) const
{
    if (m_length != rhs.m_length)
        return false;
    return memcmp(c_str(), rhs.c_str(), m_length) == 0;
}

bool Chars::operator!=(const Chars &rhs) const
{
    if (m_length != rhs.m_length)
        return true;
    return memcmp(c_str(), rhs.c_str(), m_length) != 0;
}

char &Chars::operator[](u32 index)
{
    DEBUG_BREAK_IF(index > m_length);
    return (is_heap ? m_data[index] : sso_data[index]);
}
const char &Chars::operator[](u32 index) const
{
    DEBUG_BREAK_IF(index > m_length);
    return (is_heap ? m_data[index] : sso_data[index]);
}

float Chars::ToFloat() const
{
    return (float)atof(c_str());
}
double Chars::ToDouble() const
{
    return atof(c_str());
}
int Chars::ToInt() const
{
    return atoi(c_str());
}
#endif