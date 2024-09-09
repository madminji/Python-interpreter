def fibonacci(n):
    if n <= 1:
        return n
    else:
        return fibonacci(n-1) + fibonacci(n-2)

n = 0
while n <= 7:
    print(fibonacci(n))
    n += 1

def a(m):
    t = 3
    def k(g):
        return g
    def b(n):
        def b(l):
            return l
        def c(p):
            return m + n + p + t
        return c(3) + b(10)
    return b(4) + k(5)
print(a(3))