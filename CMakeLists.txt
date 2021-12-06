cmake_minimum_required(VERSION 3.14)

project(RaychelScript VERSION 0.0.1 LANGUAGES CXX)

option(RAYCHELSCRIPT_BUILD_LEXER "Build the RaychelScript lexer" ON)
option(RAYCHELSCRIPT_BUILD_PARSER "Build the RaychelScript parser" ON)
option(RAYCHELSCRIPT_BUILD_OPTIMIZER "Build the RaychelScript optimizer" ON)
option(RAYCHELSCRIPT_BUILD_INTERPRETER "Build the RaychelScript interpreter" ON)
option(RAYCHELSCRIPT_BUILD_ASSEMBLER "Build the RASM assembler" OFF)
option(RAYCHELSCRIPT_BUILD_VM "Build the RASM virtual machine" OFF)

option(RAYCHELSCRIPT_BUILD_TESTS "Build Unit tests" ON)

option(RAYCHELSCRIPT_BUILD_TOOLCHAIN "Build the entire RaychelScript toolchain" OFF)

if(${RAYCHELSCRIPT_BUILD_TOOLCHAIN})
    set(RAYCHELSCRIPT_BUILD_LEXER ON)
    set(RAYCHELSCRIPT_BUILD_PARSER ON)
    set(RAYCHELSCRIPT_BUILD_OPTIMIZER ON)
    set(RAYCHELSCRIPT_BUILD_INTERPRETER ON)
    set(RAYCHELSCRIPT_BUILD_ASSEMBLER ON)
    set(RAYCHELSCRIPT_BUILD_VM ON)
endif()

add_subdirectory(shared)

if(${RAYCHELSCRIPT_BUILD_LEXER})
    message(STATUS "Adding RaychelScript Lexer...")
    add_subdirectory(Lexer)
endif()

if(${RAYCHELSCRIPT_BUILD_PARSER})
    message(STATUS "Adding RaychelScript Parser...")
    add_subdirectory(Parser)
endif()

if(${RAYCHELSCRIPT_BUILD_OPTIMIZER})
    message(STATUS "Adding RaychelScript Optimizer...")
    add_subdirectory(Optimizer)
endif()

if(${RAYCHELSCRIPT_BUILD_INTERPRETER})
    message(STATUS "Adding RaychelScript Interpreter...")
    add_subdirectory(Interpreter)
endif()