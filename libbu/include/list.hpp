#pragma once
#include "config.hpp"
#include <cassert>
#include <cstring>


template <typename K, typename V>
struct List
{
  struct Entry
  {
    K key;
    V value;
  };

  Entry *entries = nullptr;
  size_t count = 0;
  size_t capacity = 0;

  List() {}
  ~List() { destroy(); }

  List(const List &) = delete;
  List &operator=(const List &) = delete;

  void destroy()
  {
    if (entries)
    {
      aFree(entries);
      entries = nullptr;
    }
    count = 0;
    capacity = 0;
  }

 
  FORCE_INLINE int findIndex(const K &key) const
  {
    // Unroll primeiros 8 (cabe em 1 cache line!)
    if (count > 0 && entries[0].key == key) return 0;
    if (count > 1 && entries[1].key == key) return 1;
    if (count > 2 && entries[2].key == key) return 2;
    if (count > 3 && entries[3].key == key) return 3;
    if (count > 4 && entries[4].key == key) return 4;
    if (count > 5 && entries[5].key == key) return 5;
    if (count > 6 && entries[6].key == key) return 6;
    if (count > 7 && entries[7].key == key) return 7;
    
    // Loop para resto (nao quremos chega aqui)
    for (size_t i = 8; i < count; i++)
    {
      if (entries[i].key == key)
        return (int)i;
    }

    return -1;
  }


  FORCE_INLINE void ensureCapacity(size_t needed)
  {
    if (needed <= capacity) return;

    size_t newCap = capacity == 0 ? 8 : capacity * 2;
    Entry *newEntries = (Entry *)aAlloc(newCap * sizeof(Entry));
    
    if (entries)
    {
      memcpy(newEntries, entries, count * sizeof(Entry));
      aFree(entries);
    }

    entries = newEntries;
    capacity = newCap;
  }
 

  FORCE_INLINE bool set(const K &key, const V &value)
  {
    int idx = findIndex(key); 
    if (idx >= 0)
    {
      entries[idx].value = value;
      return false;
    }

    
    ensureCapacity(count + 1);
    entries[count].key = key;
    entries[count].value = value;
    count++;
    
    return true;
  }

  FORCE_INLINE bool set_move(const K &key, V &&value)
  {
    int idx = findIndex(key);
    
    if (idx >= 0)
    {
      entries[idx].value = value;
      return false;
    }

    ensureCapacity(count + 1);
    entries[count].key = key;
    entries[count].value = value;
    count++;
    
    return true;
  }

  FORCE_INLINE bool set_get(const K &key, const V &value, V *out)
  {
    int idx = findIndex(key);
    
    if (idx >= 0)
    {
      *out = entries[idx].value;
      entries[idx].value = value;
      return false;
    }

    ensureCapacity(count + 1);
    entries[count].key = key;
    entries[count].value = value;
    count++;
    
    return true;
  }

  FORCE_INLINE bool get(const K &key, V *out) const
  {
    int idx = findIndex(key);
    if (idx < 0) return false;
    *out = entries[idx].value;
    return true;
  }

  FORCE_INLINE V *getPtr(const K &key)
  {
    int idx = findIndex(key);
    return idx >= 0 ? &entries[idx].value : nullptr;
  }

  FORCE_INLINE const V *getPtr(const K &key) const
  {
    int idx = findIndex(key);
    return idx >= 0 ? &entries[idx].value : nullptr;
  }

  FORCE_INLINE bool contains(const K &key) const
  {
    return findIndex(key) >= 0;
  }

  FORCE_INLINE bool exist(const K &key) const
  {
    return findIndex(key) >= 0;
  }

    // pop & swap
  bool erase(const K &key)
  {
    int idx = findIndex(key);
    if (idx < 0) return false;

    if ((size_t)idx < count - 1)
    {
      entries[idx] = entries[count - 1];
    }

    count--;
    return true;
  }

  bool erase_ordered(const K &key)
{
    int idx = findIndex(key);
    if (idx < 0) return false;

    // Move tudo o que está à frente um passo para trás
    // É mais lento que o swap, mas mantém a ordem [A, C, D]
    size_t toMove = count - 1 - idx;
    if (toMove > 0)
    {
        memmove(&entries[idx], &entries[idx + 1], toMove * sizeof(Entry));
    }

    count--;
    return true;
}

  void clear()
  {
    count = 0;
  }

  // Iterar (ordem de inserção, não alfabética)
  template <typename Fn>
  void forEach(Fn fn) const
  {
    for (size_t i = 0; i < count; i++)
    {
      fn(entries[i].key, entries[i].value);
    }
  }

  template <typename Fn>
  void forEachWhile(Fn fn) const
  {
    for (size_t i = 0; i < count; i++)
    {
      if (!fn(entries[i].key, entries[i].value))
        break;
    }
  }
};

// USO:
// List<String*, uint8> fields;  // SEM comparador!
//
// REQUISITO: Strings devem ser interned (mesmo texto = mesmo ponteiro)
//
// VANTAGENS:
// - 5-10x mais rápido que comparação de strings
// - Cache-friendly (64 bytes = 8 entries em 1 cache line)
// - Sem overhead de ordenação
// - Perfeito para fields de structs