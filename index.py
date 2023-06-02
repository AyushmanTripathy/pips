def fibb(target, x, a, b):
    if (x == target): return a
    return fibb(target, x + 1, a + b, a)

print(fibb(30, 1, 1, 0))
