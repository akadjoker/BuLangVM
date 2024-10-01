#pragma once


#include <cstring> // std::strlen, std::memcpy, std::memset
#include <utility> // std::move, std::swap
#include <cstdio>
#include <cstddef>//ptrdiff_t
#include <cstdint>
#include <cctype> // 
#include <cstdlib>
#include <new>

#if defined( _MSC_VER )
    #pragma warning(disable : 4514)// unreferenced inline removed
    #pragma warning(disable : 4820)// padding added
#endif


typedef unsigned char   u8;
typedef  char           c8;
typedef unsigned short  u16;
typedef short           s16;
typedef unsigned int    u32;
typedef int             s32;
typedef long            s64;
typedef unsigned long   u64;




#if defined(_DEBUG)
#include <assert.h>
//#define DEBUG_BREAK_IF(_CONDITION_) assert(!(_CONDITION_));
#define DEBUG_BREAK_IF(condition) if (condition) { fprintf(stderr, "Debug break: %s at %s:%d\n", #condition, __FILE__, __LINE__); std::exit(EXIT_FAILURE); }

#define ASSERT(condition, message)                                           \
      do                                                                       \
      {                                                                        \
        if (!(condition))                                                      \
        {                                                                      \
          fprintf(stderr, "[%s:%d] Assert failed in %s(): %s\n",               \
              __FILE__, __LINE__, __func__, message);                          \
          abort();                                                             \
        }                                                                      \
      } while (false)

  // Indicates that we know execution should never reach this point in the
  // program. In debug mode, we assert this fact because it's a bug to get here.
  //
  // In release mode, we use compiler-specific built in functions to tell the
  // compiler the code can't be reached. This avoids "missing return" warnings
  // in some cases and also lets it perform some optimizations by assuming the
  // code is never reached.
#define UNREACHABLE()                                                        \
      do                                                                       \
      {                                                                        \
        fprintf(stderr, "[%s:%d] This code should not be reached in %s()\n",   \
            __FILE__, __LINE__, __func__);                                     \
        abort();                                                               \
      } while (false)


#else
#define DEBUG_BREAK_IF(_CONDITION_)

#define ASSERT(condition, message) do { } while (false)

// Tell the compiler that this part of the code will never be reached.
#if defined( _MSC_VER )

#define UNREACHABLE() __assume(0)
#elif (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5))
#define UNREACHABLE() __builtin_unreachable()
#else
#define UNREACHABLE()
#endif

#endif




inline size_t CalculateCapacityGrow(size_t capacity, size_t minCapacity)
{
    if (capacity < minCapacity)
        capacity = minCapacity;
    if (capacity < 8)
    {
        capacity = 8;
    }
    else
    {
        // Round up to the next power of 2 and multiply by 2 (http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2)
        capacity--;
        capacity |= capacity >> 1;
        capacity |= capacity >> 2;
        capacity |= capacity >> 4;
        capacity |= capacity >> 8;
        capacity |= capacity >> 16;
        capacity++;
    }
    return capacity;
}




template <typename T>
class Allocator
{


public:
    T* allocate(size_t n)
    {
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }

    void deallocate(T* p, size_t n)
    {
        (void)n;
        ::operator delete(p);
    }

    void deallocate(T* ptr)
	{
		operator delete(ptr);
	}

    template <typename... Args>
    void construct(T* p, Args &&...args)
    {

        new (p) T(std::forward<Args>(args)...);
    }

    void construct(T* ptr, const T& value)
	{
        new (ptr) T(value);
    }
    void destroy(T* p)
    {

        p->~T();
    }
};