import time

def fib(n):
    if n <= 1:
        return n
    
    a = 0
    b = 1
    c = 0
    while n > 1:
        c = a + b
        a = b
        b = c
        n = n - 1
    return b



# Testando a função Fibonacci e medindo o tempo de execução
start_time = time.perf_counter()
for i in range(0, 35):
    fib(i)
end_time = time.perf_counter()
elapsed_time = end_time - start_time
print(f"Tempo de execução: {elapsed_time:.6f} segundos")

    

