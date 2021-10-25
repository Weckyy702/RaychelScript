<h1>RaychelScript: Lightweight scripting language for mathematical operations</h1>

Disclaimer
-
This language is not going to be the new JS and it is not intended to be. This is a toy project for fun and education.

Syntax
-
The syntax is deliberately simple and focussed on mathematical operations. It is intended to be easy to read and easy to parse.

Features
-
*In its current state, the language cannot even be executed. Of course, hat will change in the future, but it will remain Turing-incomplete for a while.*

This languages intended purpose is as a scripting language for use in my engine [RaychelCPU](https://github.com/Weckyy702/RaychelCPU). Therefore, its syntax and featureset are tailored towards high-throughput mathematical operations (like shaders).

Building
-
* Dependencies
  * RaychelCore
  * RaychelLogger

* Compiler support
  * I have tested GCC 11.1.0 on linux 5.14. Clang 12 seems to have a problem with C++20s std::ranges library

* GNU/Linux
  * Run cmake, then run make and you have your binaries built in the *build/xxx* directory where xxx is the name of the module

I found a bug!!!
-
Nicley done! Please report it in the Issues tab or at weckyy702@gmail.com