"""
Python equivalent of BuLang Runtime Benchmark
For direct comparison with BuLang VM performance.
"""
import time

ITERATIONS = 5_000_000

# ----- 1. Int arithmetic loop -----
def bench_int_add(n):
    s = 0
    i = 0
    while i < n:
        s = s + i
        i = i + 1
    return s

# ----- 2. Float arithmetic loop -----
def bench_float_add(n):
    s = 0.0
    i = 0
    while i < n:
        s = s + 0.5
        i = i + 1
    return s

# ----- 3. Function call overhead -----
def identity(x):
    return x

def bench_function_calls(n):
    s = 0
    i = 0
    while i < n:
        s = s + identity(i)
        i = i + 1
    return s

# ----- 4. Conditional branching -----
def bench_conditionals(n):
    count = 0
    i = 0
    while i < n:
        if i % 2 == 0:
            count = count + 1
        i = i + 1
    return count

# ----- 5. String concat -----
def bench_string_concat(n):
    s = ""
    i = 0
    while i < n:
        s = s + "x"
        i = i + 1
    return len(s)

# ----- 6. Array push + access -----
def bench_array(n):
    arr = []
    i = 0
    while i < n:
        arr.append(i)
        i = i + 1
    s = 0
    i = 0
    while i < n:
        s = s + arr[i]
        i = i + 1
    return s

# ----- 7. Fibonacci recursive -----
def fib(n):
    if n < 2:
        return n
    return fib(n - 1) + fib(n - 2)

def bench_fib():
    return fib(30)

# =============================================
# RUN ALL
# =============================================
print("=== Python Benchmark ===")
print(f"Iterations: {ITERATIONS}")
print()

t0 = time.perf_counter()
r1 = bench_int_add(ITERATIONS)
t1 = time.perf_counter()
print(f"1. Int add loop:      {t1-t0:.4f} s")

t2 = time.perf_counter()
r2 = bench_float_add(ITERATIONS)
t3 = time.perf_counter()
print(f"2. Float add loop:    {t3-t2:.4f} s")

t4 = time.perf_counter()
r3 = bench_function_calls(ITERATIONS)
t5 = time.perf_counter()
print(f"3. Function calls:    {t5-t4:.4f} s")

t6 = time.perf_counter()
r4 = bench_conditionals(ITERATIONS)
t7 = time.perf_counter()
print(f"4. Conditionals:      {t7-t6:.4f} s")

STRING_N = 50_000
t8 = time.perf_counter()
r5 = bench_string_concat(STRING_N)
t9 = time.perf_counter()
print(f"5. String concat:     {t9-t8:.4f} s  (n={STRING_N})")

ARRAY_N = 500_000
t10 = time.perf_counter()
r6 = bench_array(ARRAY_N)
t11 = time.perf_counter()
print(f"6. Array push+read:   {t11-t10:.4f} s  (n={ARRAY_N})")

t12 = time.perf_counter()
r7 = bench_fib()
t13 = time.perf_counter()
print(f"7. fib(30):           {t13-t12:.4f} s  (={r7})")

print()
total = (t1-t0) + (t3-t2) + (t5-t4) + (t7-t6) + (t9-t8) + (t11-t10) + (t13-t12)
print(f"TOTAL:                {total:.4f} s")
