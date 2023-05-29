# PIPESCRIPT

A programming language revolving around piping.  
Pipescript is a functional, dynamically typed, interpreted, indented programming language.  
This is still a work in progress

syntax highlighting is pretty similer to rust's.  
for vim

```vimscript
autocmd BufNewFile,BufRead *.pipescript set syntax=rust
```

## installation

```bash
git clone https://github.com/AyushmanTripathy/pips
cd pips
make
```

move the `build/main` executable to your bin.

## Documentation

Pipescript is a very simple language.

### basics

#### piping

piping is used to channel output of one function into another.

```rust
print | add 1 2
```

following gives the same output, but instead of | we are using `code blocks` [].

```rust
print [add 1 2]
```

#### variables

variables in pipescript are immutable.

```rust
set "x" 10
print "factorial of" x "is" | factorial x
```

#### data types

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

#### conditional flow

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

### functions

function are everything in pipescript.  
there are two types of functions

#### fn functions

these are normal functions

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

#### def functions

these are pattern matching functions.

```rust
def foo
    0: pass "Even"
    1: pass "Odd"
    x: foo | reminder x 2

print 10 "is" | foo 10
```

return function can be used instead of pass if you like.  
for example a tail recursion for fibbonnacci can be implemented as such

```rust
def factorial
    x: factorial x 1
    1 y: pass y
    x y: factorial [add x -1] [multiply x y]
```

note pattern matching happens from top to bottom hence the `1 y` case must be
before `x y` case to work.

### buildin functions

1. boolean functions

| funtion name | argument       | description         |
| ------------ | -------------- | ------------------- |
| bool         | 1 any type     | converts to boolean |
| not          | 1 boolean type | equivalent of !     |
| eq           | 2 same types   | equivalent of ==    |
| lt           | 2 same types   | equivalent of <     |
| gt           | 2 same types   | equivalent of >     |
| and          | 2 booleans     | equivalent of &&    |
| or           | 2 booleans     | equivalent of \|\|  |

2. integer functions

| funtion name | argument   | description                         |
| ------------ | ---------- | ----------------------------------- |
| neg          | 1 integer  | gives negative of a integer         |
| add          | integers   | add all up integers                 |
| multiply     | integers   | multiplies all integers             |
| divide       | 2 integers | divides first by secound            |
| reminder     | 2 integers | reminder of first divied by secound |
| max          | integer    | gives max of the integers           |
| min          | integer    | gives min of the integers           |

3. miscellaneous functions

| funtion name | argument | description                               |
| ------------ | -------- | ----------------------------------------- |
| print        | any type | print to stdout space separated           |
| pass         | any type | gives the first input                     |
| return       | any type | returns the first input from the function |
| error        | 1 string | raises a raised error                     |

thank you for trying out pipescript!  
report any bugs on [issues](https://github.com/AyushmanTripathy/pips/issues)
