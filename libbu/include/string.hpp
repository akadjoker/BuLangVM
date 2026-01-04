#pragma once
#include "arena.hpp"
#include "types.hpp"

struct String
{
  static constexpr size_t SMALL_THRESHOLD = 23;
  static constexpr size_t IS_LONG_FLAG = 0x80000000u;

  int index;
  size_t hash;
  size_t length_and_flag;

  union
  {
    char *ptr;
    char data[24];
  };

  FORCE_INLINE bool isLong() const { return length_and_flag & IS_LONG_FLAG; }
  FORCE_INLINE size_t length() const { return length_and_flag & ~IS_LONG_FLAG; };

  FORCE_INLINE const char *chars() const { return isLong() ? ptr : data; }
  FORCE_INLINE char *chars() { return isLong() ? ptr : data; }
};

inline size_t hashString(const char *s, uint32 len)
{
  size_t h = 2166136261u;
  const uint8 *p = (const uint8 *)s;
  const uint8 *end = p + len;

  while (p != end)
  {
    h ^= *p++;
    h *= 16777619u;
  }
  return h;
}

struct IntEq
{
  bool operator()(int a, int b) const { return a == b; }
};

struct StringEq
{
  bool operator()(String *a, String *b) const
  {
    if (a == b)
      return true;
    if (a->length() != b->length())
      return false;
    return memcmp(a->chars(), b->chars(), a->length()) == 0;
  }
};

struct StringHasher
{
  size_t operator()(String *x) const { return x->hash; }
};

static inline bool compare_strings(String *a, String *b)
{
  if (a == b)
    return true;
  if (a->length() != b->length())
    return false;
  return memcmp(a->chars(), b->chars(), a->length()) == 0;
}