#pragma once
#include "list.hpp"
#include <cassert>

namespace BuLangVM {

struct TestList {
    static void run() {
        test_insertion();
        test_lookup();
        test_erase();
        test_erase_ordered();
        test_get_ptr();
        test_foreach();
        test_capacity_growth();
    }

    static void test_insertion() {
        List<const char*, int> list;
        const char* key1 = "key1";
        const char* key2 = "key2";
        
        assert(list.set(key1, 10) == true);   // New insertion
        assert(list.set(key1, 20) == false);  // Update existing
        assert(list.count == 1);
        
        assert(list.set(key2, 30) == true);
        assert(list.count == 2);
        list.destroy();
    }

    static void test_lookup() {
        List<const char*, int> list;
        const char* key1 = "key1";
        
        assert(list.set(key1, 42) == true);
        assert(list.contains(key1) == true);
        assert(list.exist(key1) == true);
        
        int val = 0;
        assert(list.get(key1, &val) == true);
        assert(val == 42);
        list.destroy();
    }

    static void test_erase() {
        List<const char*, int> list;
        const char* k1 = "k1";
        const char* k2 = "k2";
        const char* k3 = "k3";
        
        list.set(k1, 1);
        list.set(k2, 2);
        list.set(k3, 3);
        assert(list.count == 3);
        
        assert(list.erase(k2) == true);
        assert(list.count == 2);
        assert(list.contains(k2) == false);
        assert(list.erase(k2) == false);  // Already removed
        list.destroy();
    }

    static void test_erase_ordered() {
        List<const char*, int> list;
        const char* k1 = "k1";
        const char* k2 = "k2";
        const char* k3 = "k3";
        
        list.set(k1, 1);
        list.set(k2, 2);
        list.set(k3, 3);
        
        assert(list.erase_ordered(k2) == true);
        assert(list.count == 2);
        assert(list.entries[0].key == k1);
        assert(list.entries[1].key == k3);
        list.destroy();
    }

    static void test_get_ptr() {
        List<const char*, int> list;
        const char* key = "test";
        
        list.set(key, 100);
        int* ptr = list.getPtr(key);
        assert(ptr != nullptr);
        assert(*ptr == 100);
        
        *ptr = 200;
        int val = 0;
        list.get(key, &val);
        assert(val == 200);
        list.destroy();
    }

    static void test_foreach() {
        List<const char*, int> list;
        list.set("a", 1);
        list.set("b", 2);
        list.set("c", 3);
        
        int sum = 0;
        list.forEach([&](const char* k, int v) { sum += v; });
        assert(sum == 6);
        list.destroy();
    }

    static void test_capacity_growth() {
        List<const char*, int> list;
        assert(list.capacity == 0);
        
        for (int i = 0; i < 20; i++) {
            const char* key = (const char*)(intptr_t)i;
            list.set(key, i);
        }
        
        assert(list.count == 20);
        assert(list.capacity >= 20);
        list.destroy();
    }
};

} // namespace BuLangVM