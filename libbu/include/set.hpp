/**
 * @class HashSet
 * @brief A generic hash set implementation using open addressing with linear probing.
 *
 * @tparam K The key type to be stored in the set.
 * @tparam Hasher A callable type that computes hash values for keys of type K.
 * @tparam Eq A callable type that compares two keys for equality.
 *
 * @details
 * This hash set uses open addressing with linear probing to handle collisions.
 * Tombstones are used to mark deleted entries, allowing iteration to find both
 * new and existing entries during insertion. The hash table automatically grows
 * when the load factor exceeds MAX_LOAD (0.75).
 *
 * The capacity is always maintained as a power of two to allow fast modulo
 * operations using bitwise AND with the mask.
 *
 * @note This class is non-copyable and non-assignable.
 * @note The key type K must be a trivially copyable (POD) type.
 * @note Memory is managed through aAlloc() and aFree() functions.
 *
 * @member entries Pointer to the dynamically allocated entry array.
 * @member capacity The current capacity of the hash table (always power of 2).
 * @member count The number of active (FILLED) entries in the set.
 * @member tombstones The number of deleted entries marked as TOMBSTONE.
 * @member MAX_LOAD Maximum load factor threshold for growth (0.75).
 *
 * @see insert() - Add a key to the set.
 * @see contains() - Check if a key exists in the set.
 * @see erase() - Remove a key from the set.
 * @see clear() - Remove all entries from the set.
 * @see forEach() - Iterate over all active entries.
 * @see forEachWhile() - Iterate with early exit capability.
 * @see destroy() - Release all allocated memory.
 */
#pragma once
#include "config.hpp"
#include <cassert>
#include <cstring>

template <typename K, typename Hasher, typename Eq>
struct HashSet
{
  enum State : uint8
  {
    EMPTY = 0,      // Explicit for memset
    FILLED = 1,
    TOMBSTONE = 2
  };

  struct Entry
  {
    K key;
    size_t hash;
    State state;
  };

  Entry *entries = nullptr;
  size_t capacity = 0;
  size_t count = 0;
  size_t tombstones = 0;

  static constexpr float MAX_LOAD = 0.75f;

  HashSet() {}
  
  ~HashSet() { destroy(); }

  HashSet(const HashSet &) = delete;
  HashSet &operator=(const HashSet &) = delete;

  void destroy()
  {
    if (!entries)
      return;
    aFree(entries);
    entries = nullptr;
    capacity = count = tombstones = 0;
  }

  size_t mask() const { return capacity - 1; }

  Entry *findSlot(const K &key, size_t hash)
  {
    size_t index = hash & mask();
    Entry *tomb = nullptr;

    for (;;)
    {
      Entry *e = &entries[index];

      if (e->state == EMPTY)
      {
        return tomb ? tomb : e;
      }

      if (e->state == TOMBSTONE)
      {
        if (!tomb)
          tomb = e;
      }
      else if (e->hash == hash && Eq{}(e->key, key))
      {
        return e;
      }

      index = (index + 1) & mask();
    }
  }

  Entry *findFilled(const K &key, size_t hash) const
  {
    if (capacity == 0)
      return nullptr;
    size_t index = hash & mask();

    for (;;)
    {
      Entry *e = &entries[index];
      if (e->state == EMPTY)
        return nullptr;
      if (e->state == FILLED && e->hash == hash && Eq{}(e->key, key))
        return e;
      index = (index + 1) & mask();
    }
  }

  void adjustCapacity(size_t newCap)
  {
    // Ensure power-of-two
    assert((newCap & (newCap - 1)) == 0 && "Capacity must be power of 2");
    
    Entry *old = entries;
    size_t oldCap = capacity;

    entries = (Entry *)aAlloc(newCap * sizeof(Entry));
    std::memset(entries, 0, newCap * sizeof(Entry)); // State = EMPTY = 0

    capacity = newCap;
    count = 0;
    tombstones = 0;

    if (old)
    {
      for (size_t i = 0; i < oldCap; i++)
      {
        Entry *e = &old[i];
        if (e->state == FILLED)
        {
          Entry *dst = findSlot(e->key, e->hash);
          std::memcpy(dst, e, sizeof(Entry)); // POD copy
          count++;
        }
      }
      aFree(old);
    }
  }

  void maybeGrow()
  {
    if (capacity == 0 || (count + tombstones + 1) > capacity * MAX_LOAD)
    {
      size_t newCap = capacity == 0 ? 16 : capacity * 2;
      adjustCapacity(newCap);
    }
  }

  bool insert(const K &key)
  {
    maybeGrow();
    size_t h = Hasher{}(key);
    Entry *e = findSlot(key, h);

    bool isNew = (e->state != FILLED);
    if (isNew)
    {
      if (e->state == TOMBSTONE)
        tombstones--;
      e->key = key;
      e->hash = h;
      e->state = FILLED;
      count++;
    }
    return isNew;
  }

  bool contains(const K &key) const
  {
    if (count == 0)
      return false;
    size_t h = Hasher{}(key);
    Entry *e = findFilled(key, h);
    return e != nullptr;
  }

  bool erase(const K &key)
  {
    if (count == 0)
      return false;
    size_t h = Hasher{}(key);
    Entry *e = findFilled(key, h);
    if (!e)
      return false;
    
    e->state = TOMBSTONE;
    count--;
    tombstones++;
    return true;
  }

  void clear()
  {
    if (entries)
    {
      std::memset(entries, 0, capacity * sizeof(Entry));
      count = 0;
      tombstones = 0;
    }
  }

  template <typename Fn>
  void forEach(Fn fn) const
  {
    for (size_t i = 0; i < capacity; i++)
    {
      if (entries[i].state == FILLED)
      {
        fn(entries[i].key);
      }
    }
  }

  template <typename Fn>
  void forEachWhile(Fn fn) const
  {
    for (size_t i = 0; i < capacity; i++)
    {
      if (entries[i].state == FILLED)
      {
        if (!fn(entries[i].key))
          break;
      }
    }
  }
};;
// }