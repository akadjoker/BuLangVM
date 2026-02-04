#include "interpreter.hpp"
#include <cstdio>
#include <chrono>

// Forward declaration para bindings nativos de C++
extern void registerNativeBindings(Interpreter* vm);

using namespace std::chrono;

// Ler ficheiro para string
static char* readFile(const char* path) {
    FILE* file = fopen(path, "rb");
    if (!file) {
        fprintf(stderr, "Could not open file \"%s\".\n", path);
        return nullptr;
    }

    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    char* buffer = new char[fileSize + 1];
    size_t bytesRead = fread(buffer, 1, fileSize, file);
    buffer[bytesRead] = '\0';

    fclose(file);
    return buffer;
}

// Correr um ficheiro .bu
static int runFile(const char* path) {
    char* source = readFile(path);
    if (!source) return 1;

    Interpreter vm;
    vm.registerAll();
    registerNativeBindings(&vm);  // Registrar bindings C++ nativos
    vm.run(source, false);

    delete[] source;
    return 0;
}

void benchmark(const char* name, const char* code, int iterations = 1) {
    auto start = high_resolution_clock::now();

    for (int i = 0; i < iterations; i++) {
        Interpreter vm;
        vm.registerAll();
        registerNativeBindings(&vm);  // Registrar bindings C++ nativos
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



int main(int argc, char* argv[]) {
    // Se passar um ficheiro como argumento, corre-o
    if (argc > 1) {
        return runFile(argv[1]);
    }
    printf("========================================\n");
    printf("BuLang VM Performance Benchmark\n");
    printf("========================================\n\n");

    // Teste 1: Incremento simples em loop
    benchmark("Simple increment loop (10000 iterations)", R"(
        var i = 0;
        while (i < 10000) {
            i++;
        }
    )", 10);

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

    // Teste 9: Recursão (Fibonacci)
    benchmark("Recursive Fibonacci (fib(20))", R"(
        def fib(n) {
            if (n <= 1) return n;
            return fib(n - 1) + fib(n - 2);
        }
        fib(20);
    )", 10);

    // Teste 10: Factorial recursivo
    benchmark("Recursive Factorial (fact(15))", R"(
        def fact(n) {
            if (n <= 1) return 1;
            return n * fact(n - 1);
        }
        fact(15);
    )", 10);

    // Teste 11: Array operations (mais realista)
    benchmark("Array manipulation (1000 elements)", R"(
        var arr = [];
        var i = 0;
        while (i < 1000) {
            arr.push(i);
            i++;
        }
        var sum = 0;
        i = 0;
        while (i < len(arr)) {
            sum = sum + arr[i];
            i++;
        }
    )", 10);

    // Teste 12: Objeto com propriedades (simulação de entidades)
    benchmark("Object property access (1000 iterations)", R"(
        class Entity {
            var x = 0.0;
            var y = 0.0;
            var vx = 1.0;
            var vy = 1.0;
            
            def update() {
                self.x = self.x + self.vx;
                self.y = self.y + self.vy;
            }
        }
        
        var entities = [];
        var i = 0;
        while (i < 10) {
            entities.push(Entity());
            i++;
        }
        
        i = 0;
        while (i < 100) {
            var j = 0;
            while (j < len(entities)) {
                entities[j].update();
                j++;
            }
            i++;
        }
    )", 10);

    // Teste 13: Simulação de física (partículas)
    benchmark("Physics simulation (100 particles, 100 steps)", R"(
        class Particle {
            var x = 0.0;
            var y = 0.0;
            var vx = 0.0;
            var vy = 0.0;
            
            def init(px, py) {
                self.x = px;
                self.y = py;
                self.vx = (px - 50.0) / 10.0;
                self.vy = (py - 50.0) / 10.0;
            }
            
            def update() {
                self.x = self.x + self.vx;
                self.y = self.y + self.vy;
                self.vx = self.vx * 0.99;
                self.vy = self.vy * 0.99;
            }
        }
        
        var particles = [];
        var i = 0;
        while (i < 100) {
            var p = Particle();
            p.init(i, i);
            particles.push(p);
            i++;
        }
        
        var step = 0;
        while (step < 100) {
            i = 0;
            while (i < len(particles)) {
                particles[i].update();
                i++;
            }
            step++;
        }
    )", 10);

    // Teste 14: Nested loops com arrays
    benchmark("Nested loops with arrays (100x100)", R"(
        var matrix = [];
        var i = 0;
        while (i < 100) {
            var row = [];
            var j = 0;
            while (j < 100) {
                row.push(i * j);
                j++;
            }
            matrix.push(row);
            i++;
        }
    )", 10);
 
    printf("\n========================================\n");
    printf("Benchmark complete\n");
    printf("========================================\n");
    printf("\nCompare with Python:\n");
    printf("  python3 tests/src/benchmark.py\n");

    return 0;
}
