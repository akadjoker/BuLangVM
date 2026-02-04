#pragma once
#include "set.hpp"
#include <cassert>

// Simple hashers and comparators for testing
struct IntHasher {
    size_t operator()(int key) const {
        return static_cast<size_t>(key);
    }
};

struct IntEq {
    bool operator()(int a, int b) const {
        return a == b;
    }
};

void test_hashset_insert_and_contains() {
    HashSet<int, IntHasher, IntEq> set;
    assert(set.insert(42) == true);
    assert(set.insert(42) == false);  // duplicate
    assert(set.insert(100) == true);
    assert(set.contains(42) == true);
    assert(set.contains(100) == true);
    assert(set.contains(999) == false);
    assert(set.count == 2);
    set.destroy();
}

void test_hashset_erase() {
    HashSet<int, IntHasher, IntEq> set;
    set.insert(10);
    set.insert(20);
    set.insert(30);
    assert(set.count == 3);
    assert(set.erase(20) == true);
    assert(set.erase(20) == false);  // already erased
    assert(set.contains(20) == false);
    assert(set.count == 2);
    set.destroy();
}

void test_hashset_clear() {
    HashSet<int, IntHasher, IntEq> set;
    set.insert(1);
    set.insert(2);
    set.insert(3);
    assert(set.count == 3);
    set.clear();
    assert(set.count == 0);
    assert(set.contains(1) == false);
    set.destroy();
}

void test_hashset_growth() {
    HashSet<int, IntHasher, IntEq> set;
    for (int i = 0; i < 100; i++) {
        set.insert(i);
    }
    assert(set.count == 100);
    assert(set.capacity >= 100);
    assert((set.capacity & (set.capacity - 1)) == 0);  // power of 2
    set.destroy();
}

void test_hashset_foreach() {
    HashSet<int, IntHasher, IntEq> set;
    set.insert(5);
    set.insert(10);
    set.insert(15);
    int sum = 0;
    set.forEach([&sum](int key) { sum += key; });
    assert(sum == 30);
    set.destroy();
}

void test_hashset_foreach_while() {
    HashSet<int, IntHasher, IntEq> set;
    for (int i = 0; i < 50; i++) {
        set.insert(i);
    }
    int count = 0;
    set.forEachWhile([&count](int key) {
        count++;
        return count < 10;
    });
    assert(count == 10);
    set.destroy();
}

void test_hashset_empty() {
    HashSet<int, IntHasher, IntEq> set;
    assert(set.count == 0);
    assert(set.contains(1) == false);
    assert(set.erase(1) == false);
    set.destroy();
}

void test_hashset_tombstones() {
    HashSet<int, IntHasher, IntEq> set;
    for (int i = 0; i < 10; i++) {
        set.insert(i);
    }
    for (int i = 0; i < 5; i++) {
        set.erase(i);
    }
    assert(set.count == 5);
    assert(set.tombstones == 5);
    set.destroy();
}

void run_all_hashset_tests() {
    test_hashset_insert_and_contains();
    test_hashset_erase();
    test_hashset_clear();
    test_hashset_growth();
    test_hashset_foreach();
    test_hashset_foreach_while();
    test_hashset_empty();
    test_hashset_tombstones();
}