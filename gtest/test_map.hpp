#pragma once

#include "map.hpp"
#include <cassert>
#include <cstdio>

// Basic hashers and equality comparators for testing
struct IntHasher {
    size_t operator()(int k) const { return static_cast<size_t>(k); }
};

struct IntEq {
    bool operator()(int a, int b) const { return a == b; }
};

void test_map_basic_set_get() {
    HashMap<int, int, IntHasher, IntEq> map;
    
    map.set(1, 100);
    int val;
    assert(map.get(1, &val) && val == 100);
    
    map.set(1, 200);
    assert(map.get(1, &val) && val == 200);
    
    map.destroy();
}

void test_map_multiple_entries() {
    HashMap<int, int, IntHasher, IntEq> map;
    
    for (int i = 0; i < 100; i++) {
        map.set(i, i * 10);
    }
    
    for (int i = 0; i < 100; i++) {
        int val;
        assert(map.get(i, &val) && val == i * 10);
    }
    
    map.destroy();
}

void test_map_erase() {
    HashMap<int, int, IntHasher, IntEq> map;
    
    map.set(1, 100);
    map.set(2, 200);
    assert(map.erase(1));
    assert(!map.get(1, nullptr));
    assert(map.get(2, nullptr));
    
    map.destroy();
}

void test_map_contains() {
    HashMap<int, int, IntHasher, IntEq> map;
    
    map.set(5, 50);
    assert(map.contains(5));
    assert(!map.contains(10));
    
    map.destroy();
}

void test_map_getPtr() {
    HashMap<int, int, IntHasher, IntEq> map;
    
    map.set(1, 100);
    int* ptr = map.getPtr(1);
    assert(ptr && *ptr == 100);
    *ptr = 200;
    assert(map.get(1, nullptr) && *map.getPtr(1) == 200);
    
    map.destroy();
}

void test_map_clear() {
    HashMap<int, int, IntHasher, IntEq> map;
    
    map.set(1, 10);
    map.set(2, 20);
    map.clear();
    
    assert(!map.get(1, nullptr));
    assert(!map.get(2, nullptr));
    assert(map.count == 0);
    
    map.destroy();
}

void test_map_forEach() {
    HashMap<int, int, IntHasher, IntEq> map;
    
    map.set(1, 10);
    map.set(2, 20);
    map.set(3, 30);
    
    int sum = 0;
    map.forEach([&sum](int k, int v) {
        sum += v;
    });
    assert(sum == 60);
    
    map.destroy();
}

void test_map_forEachWhile() {
    HashMap<int, int, IntHasher, IntEq> map;
    
    map.set(1, 10);
    map.set(2, 20);
    map.set(3, 30);
    
    int count = 0;
    map.forEachWhile([&count](int k, int v) {
        count++;
        return count < 2;
    });
    assert(count == 2);
    
    map.destroy();
}

void test_map_set_get() {
    HashMap<int, int, IntHasher, IntEq> map;
    
    int old;
    bool isNew = map.set_get(1, 100, &old);
    assert(isNew);
    
    isNew = map.set_get(1, 200, &old);
    assert(!isNew && old == 100);
    
    map.destroy();
}

void test_map_capacity_growth() {
    HashMap<int, int, IntHasher, IntEq> map;
    
    size_t oldCap = map.capacity;
    for (int i = 0; i < 50; i++) {
        map.set(i, i);
    }
    assert(map.capacity > oldCap);
    
    map.destroy();
}

void run_all_map_tests() {
    test_map_basic_set_get();
    test_map_multiple_entries();
    test_map_erase();
    test_map_contains();
    test_map_getPtr();
    test_map_clear();
    test_map_forEach();
    test_map_forEachWhile();
    test_map_set_get();
    test_map_capacity_growth();
    
    printf("All HashMap tests passed!\n");
}