#include "interpreter.hpp"
#include <cstdio>
#include <chrono>

using namespace std::chrono;

int main() {
    printf("=== Performance Test ===\n\n");
    
    // Test 1: Simple loop
    {
        auto start = high_resolution_clock::now();
        Interpreter vm;
        vm.registerAll();
        vm.run(R"(
            var i = 0;
            while (i < 10000) {
                i++;
            }
        )", false);
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start).count();
        printf("Test 1 - Simple loop (10k): %lld μs\n", (long long)duration);
    }
    
    // Test 2: Multiple globals
    {
        auto start = high_resolution_clock::now();
        Interpreter vm;
        vm.registerAll();
        vm.run(R"(
            var a = 0; var b = 0; var c = 0; var d = 0; var e = 0;
            var i = 0;
            while (i < 1000) {
                a++; b++; c++; d++; e++;
                i++;
            }
        )", false);
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start).count();
        printf("Test 2 - Multiple globals: %lld μs\n", (long long)duration);
    }
    
    // Test 3: Prefix increment
    {
        auto start = high_resolution_clock::now();
        Interpreter vm;
        vm.registerAll();
        vm.run(R"(
            var sum = 0;
            var i = 0;
            while (i < 5000) {
                sum = sum + ++i;
            }
        )", false);
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start).count();
        printf("Test 3 - Prefix increment: %lld μs\n", (long long)duration);
    }
    
    // Test 4: Postfix increment
    {
        auto start = high_resolution_clock::now();
        Interpreter vm;
        vm.registerAll();
        vm.run(R"(
            var sum = 0;
            var i = 0;
            while (i < 5000) {
                sum = sum + i++;
            }
        )", false);
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start).count();
        printf("Test 4 - Postfix increment: %lld μs\n", (long long)duration);
    }
    
    printf("\nDone!\n");
    return 0;
}
