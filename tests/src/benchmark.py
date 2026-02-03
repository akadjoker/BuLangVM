#!/usr/bin/env python3
import time

def benchmark(name, func, iterations=10):
    start = time.perf_counter()
    
    for _ in range(iterations):
        func()
    
    end = time.perf_counter()
    duration = (end - start) * 1_000_000  # convert to microseconds
    
    print(f"[BENCH] {name}: {duration:.0f} μs ({iterations} iterations, {duration/iterations:.2f} μs/iter)")

def test_simple_increment():
    i = 0
    while i < 10000:
        i += 1

def test_multiple_globals():
    a = b = c = d = e = 0
    i = 0
    while i < 1000:
        a += 1; b += 1; c += 1; d += 1; e += 1
        i += 1

def test_local_function():
    def test():
        sum_val = 0
        i = 0
        while i < 1000:
            local = i
            sum_val = sum_val + local
            i += 1
        return sum_val
    test()

def test_arithmetic():
    sum_val = 0
    i = 0
    while i < 5000:
        sum_val = sum_val + i * 2 - 1
        i += 1

def test_many_globals():
    v0=v1=v2=v3=v4=v5=v6=v7=v8=v9=0
    v10=v11=v12=v13=v14=v15=v16=v17=v18=v19=0
    v20=v21=v22=v23=v24=v25=v26=v27=v28=v29=0
    v30=v31=v32=v33=v34=v35=v36=v37=v38=v39=0
    v40=v41=v42=v43=v44=v45=v46=v47=v48=v49=0
    
    i = 0
    while i < 100:
        v0+=1; v10+=1; v20+=1; v30+=1; v40+=1
        i += 1

def fib(n):
    if n <= 1:
        return n
    return fib(n - 1) + fib(n - 2)

def test_fibonacci():
    fib(20)

def fact(n):
    if n <= 1:
        return 1
    return n * fact(n - 1)

def test_factorial():
    fact(15)

def test_array_manipulation():
    arr = []
    i = 0
    while i < 1000:
        arr.append(i)
        i += 1
    
    sum_val = 0
    i = 0
    while i < len(arr):
        sum_val = sum_val + arr[i]
        i += 1

def test_object_properties():
    class Entity:
        def __init__(self):
            self.x = 0.0
            self.y = 0.0
            self.vx = 1.0
            self.vy = 1.0
        
        def update(self):
            self.x = self.x + self.vx
            self.y = self.y + self.vy
    
    entities = []
    i = 0
    while i < 10:
        entities.append(Entity())
        i += 1
    
    i = 0
    while i < 100:
        j = 0
        while j < len(entities):
            entities[j].update()
            j += 1
        i += 1

def test_physics_simulation():
    class Particle:
        def __init__(self, px, py):
            self.x = px
            self.y = py
            self.vx = (px - 50.0) / 10.0
            self.vy = (py - 50.0) / 10.0
        
        def update(self):
            self.x = self.x + self.vx
            self.y = self.y + self.vy
            self.vx = self.vx * 0.99
            self.vy = self.vy * 0.99
    
    particles = []
    i = 0
    while i < 100:
        particles.append(Particle(i, i))
        i += 1
    
    step = 0
    while step < 100:
        i = 0
        while i < len(particles):
            particles[i].update()
            i += 1
        step += 1

def test_nested_loops():
    matrix = []
    i = 0
    while i < 100:
        row = []
        j = 0
        while j < 100:
            row.append(i * j)
            j += 1
        matrix.append(row)
        i += 1

if __name__ == "__main__":
    print("========================================")
    print("Python Performance Benchmark")
    print("========================================\n")
    
    benchmark("Simple increment loop (10000 iterations)", test_simple_increment)
    benchmark("Multiple global variables", test_multiple_globals)
    benchmark("Local variables in function", test_local_function)
    benchmark("Arithmetic operations", test_arithmetic)
    benchmark("Many global variables (50)", test_many_globals)
    benchmark("Recursive Fibonacci (fib(20))", test_fibonacci)
    benchmark("Recursive Factorial (fact(15))", test_factorial)
    benchmark("Array manipulation (1000 elements)", test_array_manipulation)
    benchmark("Object property access (1000 iterations)", test_object_properties)
    benchmark("Physics simulation (100 particles, 100 steps)", test_physics_simulation)
    benchmark("Nested loops with arrays (100x100)", test_nested_loops)
    
    print("\n========================================")
    print("Benchmark complete")
    print("========================================")
