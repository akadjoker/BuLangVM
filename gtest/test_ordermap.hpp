#pragma once
#include "ordermap.hpp"
#include <cassert>
#include <cstring>

// Simple comparator for testing
struct IntCmp {
    bool operator()(int a, int b) const { return a < b; }
};

struct StringCmp {
    bool operator()(const char* a, const char* b) const { 
        return strcmp(a, b) < 0; 
    }
};

void test_ordermap_basic_set_get() {
    Map<int, int, IntCmp> map;
    assert(map.set(5, 50) == true);
    assert(map.set(3, 30) == true);
    assert(map.set(7, 70) == true);
    
    int val;
    assert(map.get(5, &val) && val == 50);
    assert(map.get(3, &val) && val == 30);
    assert(map.get(7, &val) && val == 70);
    assert(map.get(99, &val) == false);
}

void test_ordermap_update_existing() {
    Map<int, int, IntCmp> map;
    assert(map.set(5, 50) == true);
    assert(map.set(5, 500) == false);
    
    int val;
    assert(map.get(5, &val) && val == 500);
    assert(map.count == 1);
}

void test_ordermap_contains() {
    Map<int, int, IntCmp> map;
    map.set(10, 100);
    map.set(20, 200);
    
    assert(map.contains(10) == true);
    assert(map.contains(20) == true);
    assert(map.contains(30) == false);
    assert(map.exist(10) == true);
}

void test_ordermap_getptr() {
    Map<int, int, IntCmp> map;
    map.set(5, 50);
    
    int* ptr = map.getPtr(5);
    assert(ptr != nullptr && *ptr == 50);
    *ptr = 500;
    
    int val;
    assert(map.get(5, &val) && val == 500);
    
    assert(map.getPtr(99) == nullptr);
}

void test_ordermap_set_get() {
    Map<int, int, IntCmp> map;
    assert(map.set(5, 50) == true);
    
    int oldVal;
    assert(map.set_get(5, 500, &oldVal) == false);
    assert(oldVal == 50);
    
    int val;
    assert(map.get(5, &val) && val == 500);
    assert(map.count == 1);
}

void test_ordermap_sorted_iteration() {
    Map<int, int, IntCmp> map;
    map.set(5, 50);
    map.set(1, 10);
    map.set(9, 90);
    map.set(3, 30);
    
    int last = -1;
    map.forEach([&last](int k, int v) {
        assert(k > last);
        last = k;
    });
}

void test_ordermap_foreach_while() {
    Map<int, int, IntCmp> map;
    map.set(1, 10);
    map.set(2, 20);
    map.set(3, 30);
    map.set(4, 40);
    
    int count = 0;
    map.forEachWhile([&count](int k, int v) {
        count++;
        return count < 3;
    });
    assert(count == 3);
}

void test_ordermap_clear() {
    Map<int, int, IntCmp> map;
    map.set(1, 10);
    map.set(2, 20);
    map.set(3, 30);
    assert(map.count == 3);
    
    map.clear();
    assert(map.count == 0);
    assert(map.root == nullptr);
}

void test_ordermap_empty_operations() {
    Map<int, int, IntCmp> map;
    
    int val;
    assert(map.get(1, &val) == false);
    assert(map.contains(1) == false);
    assert(map.getPtr(1) == nullptr);
    assert(map.count == 0);
}

void test_ordermap_string_keys() {
    Map<const char*, int, StringCmp> map;
    assert(map.set("apple", 1) == true);
    assert(map.set("banana", 2) == true);
    assert(map.set("cherry", 3) == true);
    
    int val;
    assert(map.get("banana", &val) && val == 2);
    assert(map.count == 3);
}

void test_ordermap_move_semantics() {
    Map<int, int, IntCmp> map;
    assert(map.set_move(5, 50) == true);
    assert(map.set_move(3, 30) == true);
    
    int val;
    assert(map.get(5, &val) && val == 50);
    assert(map.get(3, &val) && val == 30);
}

void test_ordermap_multiple_insertions() {
    Map<int, int, IntCmp> map;
    for (int i = 0; i < 100; i++) {
        assert(map.set(i, i * 10) == true);
    }
    assert(map.count == 100);
    
    for (int i = 0; i < 100; i++) {
        int val;
        assert(map.get(i, &val) && val == i * 10);
    }
}

void run_all_ordermap_tests() {
    test_ordermap_basic_set_get();
    test_ordermap_update_existing();
    test_ordermap_contains();
    test_ordermap_getptr();
    test_ordermap_set_get();
    test_ordermap_sorted_iteration();
    test_ordermap_foreach_while();
    test_ordermap_clear();
    test_ordermap_empty_operations();
    test_ordermap_string_keys();
    test_ordermap_move_semantics();
    test_ordermap_multiple_insertions();
}