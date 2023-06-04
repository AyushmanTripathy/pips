# PIPESCRIPT

A programming language revolving around piping.  
Pipescript is a functional, dynamically weakly typed, interpreted, indented programming language.  
This is still a work in progress

### Believes

1. useless complexity is waste of time.
1. using functions for everything.
1. human readable code.
1. piping is a great idea.

plz report any bugs by creating an issue.  
for getting an idea about performance of pipescript [check out.](./extras/stats.md)

## Features

1. piping
1. pattern matching functions
1. pure functions
1. immutabillity by default
1. higher order functions (coming soon)

by pure functions i mean functions are idempotent and have no side effects.
note, printing to stdout is not considered a side effect for simplicity reasons.

## Installation

```bash
git clone https://github.com/AyushmanTripathy/pips
cd pips
make clean install //with privilege
pips <input file>
```

to change the installation location, change the $INSTALLDIR in Makefile.  
intrested in syntax highlighting [check out.](./extras/syntax_highlighting.md)

## Documentation

Pipescript is a very simple language.
pipescript supports `#` and `//` comments

### Piping

piping is used to channel output of one function into another.

```rust
print | add 1 2
```

following gives the same output, but instead of | we are using `code blocks` [].

```rust
print [add 1 2]
```

### Variables

variables in pipescript are immutable.

```rust
set "x" 10
print "factorial of" x "is" | factorial x
```

### Data types

there are 3 data types in pipescript.

1. integer

```
set "x" 101
```

2. strings

```
set "x" "this is a example string"
```

3. boolean

```
set "x" True
set "y" False
```

4. null

```
set "x" Null
```

null value is returned by void functions.

### Conditional flow

pipescript has your well know if, else statments

```rust
if [eq number 1]:
    print "number is one"
elif [eq number 2]:
    print "number is two"
else:
    print "number is not one or two"
```

note the use of code blocks, pipes can also be used.

```rust
if | eq number 1: print "number is still one"
```

### Functions

function are king in pipescript.  
there are two types of functions

-   fn functions

these are good old functions

```rust
fn factorial x:
    if [eq x 1]: return 1
    return | multiply x | factorial [add x -1]
```

note return is also a function, so you have to pipe your output into return.  
functions can also be one liners

```rust
fn square x: pow x 2
```

-   self (tail recursions)

self keyword is used to call the parent function. this is used to mark a tail
recurison which is more efficient and faster than normal recursion.

```rust
fn factorial x y:
    if [eq x 1]: return y
    return | self [add x -1] [multiply x y]
```

note: a return should always follow self calls (tail recursion fashion).

```rust
fn factorial x:
    if [eq x 1]: return 1
    return | multiply | self [add x -1]
```

doing something like this will result in a type error or a function collapse
error (error raised when function cannot give an output).

-   def functions

these are pattern matching functions.

```python
def foo
    0: pass "Even"
    1: pass "Odd"
    x: foo | reminder x 2

print 10 "is" | foo 10
```

return function can be used instead of pass if you like.  
for example a factorial be implemented as such

```python
def factorial
    x: factorial x 1
    1 y: pass y
    x y: self [add x -1] [multiply x y]
```

note pattern matching happens from top to bottom hence the `1 y` case must be
before `x y` case to work.

for more advanded pattern we use guards, for example fibbonacci can be implemented as such.

```python
def patternFibb
  x: patternFibb x 1 1 0
  target x a b: self target [add x 1] [add a b] a
    eq target x: pass a
```

if no guards match then default statment is executed.

```python
def trickFibb
  target: self 1 1 0
  x a b: self [add x 1] [add a b] a
    eq target x: pass a
```

this is a handy little trick, note the use of self in first pattern case. when
function is called with self, variables from earlier instances are saved unless
overwritten.

execution starts with global function (global scope), hence it is possible to
return out of global scope.  
if a integer value is returned, it is used as exit code.

### Buildin Functions

1. Boolean functions

| funtion name | argument       | description         |
| ------------ | -------------- | ------------------- |
| bool         | 1 any type     | converts to boolean |
| not          | 1 boolean type | equivalent of !     |
| eq           | 2 same types   | equivalent of ==    |
| lt           | 2 same types   | equivalent of <     |
| gt           | 2 same types   | equivalent of >     |
| and          | 2 booleans     | equivalent of &&    |
| or           | 2 booleans     | equivalent of \|\|  |

2. Integer functions

| funtion name | argument   | description                         |
| ------------ | ---------- | ----------------------------------- |
| neg          | 1 integer  | gives negative of a integer         |
| add          | integers   | add all up integers                 |
| multiply     | integers   | multiplies all integers             |
| divide       | 2 integers | divides first by secound            |
| reminder     | 2 integers | reminder of first divied by secound |
| max          | integer    | gives max of the integers           |
| min          | integer    | gives min of the integers           |

3. Miscellaneous functions

| funtion name | argument | description                               |
| ------------ | -------- | ----------------------------------------- |
| print        | any type | print to stdout space separated           |
| pass         | any type | gives the first input                     |
| return       | any type | returns the first input from the function |
| error        | 1 string | raises a raised error                     |

thank you for trying out pipescript! 👍
