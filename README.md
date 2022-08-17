# RaychelScript: A Lightweight scripting language for mathematical operations
[![Build Status](https://app.travis-ci.com/Weckyy702/RaychelScript.svg?branch=main)](https://app.travis-ci.com/Weckyy702/RaychelScript)
## Disclaimer

This language is not going to be the new JS and it is not intended to be. This is a toy project for fun and education.

## :pencil2: Syntax

The syntax is deliberately simple and focused on mathematical operations. It is intended to be easy to read and easy to parse.
My good friend [plexx-dev](https://github.com/plexx-dev) has published an awesome VSCode extension for RSC syntax highlighting. [Check it out!](https://github.com/plexx-dev/rsc-syntax-highlighting)
See the [examples](#interrobang-examples) for syntax examples.

## Features

This languages intended purpose is as a scripting language for use in my engine [Raychel](https://github.com/weckyy702/raychel). Therefore, its syntax and featureset are tailored towards high-throughput mathematical operations (like shaders).

## :hammer_and_wrench: Building

### Dependencies

- [RaychelCore](https://github.com/Weckyy702/RaychelCore)
- [RaychelLogger](https://github.com/Weckyy702/RaychelLogger)
- [RaychelMath](https://github.com/Weckyy702/RaychelMath)

### Compiler support

- I have tested GCC 11.1.0 and Clang 14 on linux 5.16 and MSVC on Windows 10.
- ***I am primarily on a linux system, so changes that break Windows compatibility will likely go unnoticed. If you encounter build errors on windows, please [file a bug report](#skull-i-found-a-bug)***

### GNU/Linux

- Run cmake, then run make and you have your binaries built in the *build/xxx* directory where xxx is the name of the module

### Windows

- You will need Visual Studio with the C++ workload and CMake installed.
- Running cmake will create a RaychelScript.sln file. Open it in Visual Studio to compile the program. Due to windows limitations, you will have
    to copy the built DLLs into the executable directory (either manually or via copy_DLLs.sh)

## Note on the coding style

- I am currently exploring many different coding styles and design patterns and I am using this project to test out what feels good. So the style of the code is quite inconsistent between different modules.

## :skull: I found a bug

- Nicley done!
Please report it in the Issues tab or at weckyy702@gmail.com
## :interrobang: Examples
### Basic structure
A RaychelScript script is divided into two blocks: the config and the body block.
The config block contains metadata like input and output variables. You can add custom entry with the format `name value [values...]`. Custom config entries currently have no effect.
```
#This is a comment. The entire line is ignored
[[config]]
input a b       #These entries are required
output c        #

name block_example #a custom entry

#The body block contains the actual code
[[body]]
#Code like you can see in the next examples
```
### Constants and variables
```
#snip
let a_constant = 5 #This is a constant. It cannot be reassigned
var a_variable = 12 #This variable can be reassigned

a_variable = 5     #a_variable now holds 5
a_constant = 10    #error: cannot reassign constant 'a_constant'
```
### Operators
RaychelScript knows the following mathematical operators
```
var a
let b

#binary operators
let addition = a + b
let subtraction = a - b
let multiplication = a * b
let division = a / b
let power = a ^ b

#in-place binary operators
a += b  #increment
a -= b  #decrement
a *= b  #multiply
a /= b  #divide
a ^= b  #power

#unary operators
let unary_minus = -a
let unary_plus = +a
let absolute_value = |a|
let factorial = a!

#comparison operators
a == b
a != b
a < b
a > b
```
***Please note that operator precedence is very quirky at the moment. Always use parentheses to ensure correctness.***
### Conditional constructs
```
#snip
if *expression*
    #expression was true
else
    #expression was false. The else block is not required
endif
```
Silly way to get the absolute value of a number
```
[[config]]
input x
output out

[[body]]
if x < 0
    out = -x
else
    out = x
endif
```
### Loops
```
#snip
while *expression*
    #code
endwhile
```
Sum up all postive numbers smaller than n
```
[[config]]
input n
output out

[[body]]
var i = 0
while i < n
    out+=i
    i+=1
endwhile
```
### Functions
RaychelScript now supports functions :partying_face: ! All functions in RaychelScript are *pure* meaning they cannot change anything that is not local inside their body.
```
#snip
fn f(x) = x^2

var global_variable = 12
fn very_complicated_function(a, b, c)
    #code that cannot see global_variable
    #...
    #function arguments are also passed in as constants
    return *result* #not having a return statement in a function is an error
endfn
```
Functions can also be [overloaded](https://en.wikipedia.org/wiki/Function_overloading) on the number of parameters they take.
```
#snip
fn f(base, exponent) = base^exponent

fn f(x) = f(x, 2)

#f(1) will call the latter definition whereas f(1, 2) will call the first overload of f
```

## :scroll: License
[MIT](https://opensource.org/licenses/MIT)
