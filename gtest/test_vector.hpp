#pragma once
#include "vector.hpp"
#include <cassert>
#include <iostream>

// Mock allocation functions for testing
void* aAlloc(size_t size) { return malloc(size); }
void aFree(void* ptr) { free(ptr); }

class VectorTests
{
public:
    static void runAllTests()
    {
        testConstructor();
        testPush();
        testPop();
        testReserve();
        testInsert();
        testRemove();
        testFind();
        testContains();
        testReverse();
        testSwap();
        testClear();
        testResize();
        testMoveSemantics();
        std::cout << "All tests passed!" << std::endl;
    }

private:
    static void testConstructor()
    {
        Vector<int> v1;
        assert(v1.empty());
        assert(v1.size() == 0);
        assert(v1.capacity() >= 8);

        Vector<int> v2(16);
        assert(v2.capacity() >= 16);
    }

    static void testPush()
    {
        Vector<int> v;
        v.push(10);
        v.push(20);
        v.push(30);
        assert(v.size() == 3);
        assert(v[0] == 10);
        assert(v[1] == 20);
        assert(v[2] == 30);
    }

    static void testPop()
    {
        Vector<int> v;
        v.push(1);
        v.push(2);
        v.pop();
        assert(v.size() == 1);
        assert(v[0] == 1);
    }

    static void testReserve()
    {
        Vector<int> v;
        v.reserve(100);
        assert(v.capacity() >= 100);
    }

    static void testInsert()
    {
        Vector<int> v;
        v.push(1);
        v.push(3);
        v.insert(1, 2);
        assert(v.size() == 3);
        assert(v[0] == 1);
        assert(v[1] == 2);
        assert(v[2] == 3);
    }

    static void testRemove()
    {
        Vector<int> v;
        v.push(1);
        v.push(2);
        v.push(3);
        v.remove(1);
        assert(v.size() == 2);
        assert(v[0] == 1);
        assert(v[1] == 3);
    }

    static void testFind()
    {
        Vector<int> v;
        v.push(5);
        v.push(10);
        v.push(15);
        assert(v.find(10) == 1);
        assert(v.find(99) == -1);
    }

    static void testContains()
    {
        Vector<int> v;
        v.push(42);
        assert(v.contains(42));
        assert(!v.contains(99));
    }

    static void testReverse()
    {
        Vector<int> v;
        v.push(1);
        v.push(2);
        v.push(3);
        v.reverse();
        assert(v[0] == 3);
        assert(v[1] == 2);
        assert(v[2] == 1);
    }

    static void testSwap()
    {
        Vector<int> v;
        v.push(1);
        v.push(2);
        v.swap(0, 1);
        assert(v[0] == 2);
        assert(v[1] == 1);
    }

    static void testClear()
    {
        Vector<int> v;
        v.push(1);
        v.push(2);
        v.clear();
        assert(v.size() == 0);
        assert(v.empty());
    }

    static void testResize()
    {
        Vector<int> v;
        v.resize(5);
        assert(v.size() == 5);
        assert(v.capacity() >= 5);
    }

    static void testMoveSemantics()
    {
        Vector<int> v1;
        v1.push(1);
        v1.push(2);
        Vector<int> v2(std::move(v1));
        assert(v2.size() == 2);
        assert(v1.data() == nullptr);
    }
};