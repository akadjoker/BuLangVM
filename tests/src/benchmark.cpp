#include "interpreter.hpp"
#include <cstdio>
#include <chrono>

using namespace std::chrono;

void benchmark(const char* name, const char* code, int iterations = 1) {
    auto start = high_resolution_clock::now();
    
    for (int i = 0; i < iterations; i++) {
        Interpreter vm;
        vm.registerAll();
        vm.run(code, false);
    }
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    
    printf("[BENCH] %s: %lld μs (%d iterations, %.2f μs/iter)\n", 
           name, 
           (long long)duration.count(), 
           iterations,
           (double)duration.count() / iterations);
}

int main() {
    printf("========================================\n");
    printf("BuLang VM Performance Benchmark\n");
    printf("========================================\n\n");

    // Teste 1: Incremento simples em loop
    benchmark("Simple increment loop (10000 iterations)", R"(
        var i = 0;
        while (i < 10000) {
            i++;
        }
    )", 1);

    // Teste 2: Múltiplas variáveis globais
    benchmark("Multiple global variables", R"(
        var a = 0; var b = 0; var c = 0; var d = 0; var e = 0;
        var i = 0;
        while (i < 1000) {
            a++; b++; c++; d++; e++;
            i++;
        }
    )", 10);

    // Teste 3: Acesso a variáveis locais em função
    benchmark("Local variables in function", R"(
        def test() {
            var sum = 0;
            var i = 0;
            while (i < 1000) {
                var local = i;
                sum = sum + local;
                i++;
            }
            return sum;
        }
        test();
    )", 10);

    // Teste 4: Prefix vs Postfix
    benchmark("Prefix increment", R"(
        var i = 0;
        var sum = 0;
        while (i < 5000) {
            sum = sum + ++i;
        }
    )", 10);

    benchmark("Postfix increment", R"(
        var i = 0;
        var sum = 0;
        while (i < 5000) {
            sum = sum + i++;
        }
    )", 10);

    // Teste 5: Closures/Upvalues
    benchmark("Closure with upvalue", R"(
        def outer() {
            var x = 0;
            def inner() {
                x++;
                return x;
            }
            var i = 0;
            while (i < 1000) {
                inner();
                i++;
            }
            return x;
        }
        outer();
    )", 10);

    // Teste 6: Classes e métodos
    benchmark("Class method calls", R"(
        class Counter {
            var value = 0;
            def inc() { self.value++; }
        }
        var c = Counter();
        var i = 0;
        while (i < 1000) {
            c.inc();
            i++;
        }
    )", 10);

    // Teste 7: Operações aritméticas
    benchmark("Arithmetic operations", R"(
        var sum = 0;
        var i = 0;
        while (i < 5000) {
            sum = sum + i * 2 - 1;
            i++;
        }
    )", 10);

    // Teste 8: Muitas variáveis (stress test para constant pool)
    benchmark("Many global variables (50)", R"(
        var v0=0; var v1=0; var v2=0; var v3=0; var v4=0;
        var v5=0; var v6=0; var v7=0; var v8=0; var v9=0;
        var v10=0; var v11=0; var v12=0; var v13=0; var v14=0;
        var v15=0; var v16=0; var v17=0; var v18=0; var v19=0;
        var v20=0; var v21=0; var v22=0; var v23=0; var v24=0;
        var v25=0; var v26=0; var v27=0; var v28=0; var v29=0;
        var v30=0; var v31=0; var v32=0; var v33=0; var v34=0;
        var v35=0; var v36=0; var v37=0; var v38=0; var v39=0;
        var v40=0; var v41=0; var v42=0; var v43=0; var v44=0;
        var v45=0; var v46=0; var v47=0; var v48=0; var v49=0;
        var i = 0;
        while (i < 100) {
            v0++; v10++; v20++; v30++; v40++;
            i++;
        }
    )", 10);

    printf("\n========================================\n");
    printf("Benchmark complete\n");
    printf("========================================\n");

    return 0;
}
