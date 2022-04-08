# RaychelScript: A Lightweight scripting language for mathematical operations
[![Build Status](https://app.travis-ci.com/Weckyy702/RaychelScript.svg?branch=main)](https://app.travis-ci.com/Weckyy702/RaychelScript)
## Disclaimer

This language is not going to be the new JS and it is not intended to be. This is a toy project for fun and education.

## Syntax

The syntax is deliberately simple and focused on mathematical operations. It is intended to be easy to read and easy to parse.
My good friend [plexx-dev](https://github.com/plexx-dev) has published an awesome VSCode extension for RSC syntax highlighting. [Check it out!](https://github.com/plexx-dev/rsc-syntax-highlighting)

## Features

This languages intended purpose is as a scripting language for use in my engine [Raychel](https://github.com/weckyy702/raychel). Therefore, its syntax and featureset are tailored towards high-throughput mathematical operations (like shaders).

## Building

### Dependencies

- RaychelCore
- RaychelLogger
- RaychelMath

### Compiler support

- I have tested GCC 11.1.0 and Clang 14 on linux 5.16 and MSVC on Windows 10.

### GNU/Linux

- Run cmake, then run make and you have your binaries built in the *build/xxx* directory where xxx is the name of the module

### Windows

- You will need Visual Studio with the C++ workload and CMake installed. You also have to install the dependencies manually.
- Running cmake will create a RaychelScript.sln file. Open it in Visual Studio to compile the program. Due to windows limitations, you will have
    to copy the built DLLs into the executable directory (either manually or via copy_DLLs.sh)

## Note on the coding style

- I am currently exploring many different coding styles and design patterns and I am using this project to test out what feels good. So the style of the code is quite inconsistent between different modules.

## I found a bug

- Nicley done!
Please report it in the Issues tab or at weckyy702@gmail.com
