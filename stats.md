I tried to somewhat measure performance of pipescript by trying to calculate
30th fibbonacci number.

### Enviroment

- [hardware](https://support.hp.com/in-en/document/c06410947)
- Arch Linux with X org
- tools: valgrind and time

### Conclusions

pipescript works well with tail recursion. without `self` pipescript has very
bad performance.

## TAIL RECURSION

1. pipescript with fn function

```rust
fn tailFibb target x a b:
  if [eq target x]: return a
  return | self target [add x 1] [add a b] a

print | tailFibb 30 1 1 0
```

2. pipescript with def function

```python
def patternFibb
  o: patternFibb o 1 1 0
  t x a b: self t [add x 1] [add a b] a
    eq t x: pass a

print | patternFibb 30
```

3. python with tail recursion

```python
def fibb(target, x, a, b):
    if (x == target): return a
    return fibb(target, x + 1, a + b, a)

print(fibb(30, 1, 1, 0))
```

| subject       | time | total memory usage | allocations | frees |
| ------------- | ---- | ------------------ | ----------- | ----- |
| fn functions  | 5ms  | 33,186 bytes       | 965         | 965   |
| def functions | 5ms  | 32,001 bytes       | 802         | 802   |
| python        | 30ms | 1,608,940 bytes    | 3,407       | 3,404 |

## WITHOUT TAIL RECURSION

1. pipescript without tail recursion

```rust
fn fibb x:
  if [eq x 1]: return 1
  elif [eq x 0]: return 0
  return | add [fibb [add x -1]] [fibb [add x -2]]

print | fibb 30
```

2. python without tail recursion

```python
def fibb(x):
    if (x == 1): return 1
    elif (x == 0): return 0
    return fibb(x - 1) + fibb(x - 2)

print(fibb(30))
```

| subject    | time    | total memory usage  | allocations | frees      |
| ---------- | ------- | ------------------- | ----------- | ---------- |
| pipescript | 5,100ms | 3,146,282,843 bytes | 69,884,790  | 69,884,790 |
| python     | 260ms   | 1,608,396 bytes     | 3,408       | 3,405      |
