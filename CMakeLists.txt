cmake_minimum_required(VERSION 3.14)

project(RaychelScript VERSION 1.0.0 LANGUAGES CXX)

option(RAYCHELSCRIPT_BUILD_LEXER "Build the RaychelScript lexer" OFF)
option(RAYCHELSCRIPT_BUILD_PARSER "Build the RaychelScript parser" OFF)
option(RAYCHELSCRIPT_BUILD_INTERPRETER "Build the RaychelScript interpreter" OFF)
option(RAYCHELSCRIPT_BUILD_RASM_LIB "Build the RASM library" OFF)
option(RAYCHELSCRIPT_BUILD_ASSEMBLER "Build the RASM assembler" OFF)
option(RAYCHELSCRIPT_BUILD_VM "Build the RASM virtual machine" OFF)

option(RAYCHELSCRIPT_BUILD_TESTS "Build Unit tests" ON)

option(RAYCHELSCRIPT_ENABLE_ASAN "Enable ASAN instrumentation" ON)
option(RAYCHELSCRIPT_ENABLE_UBSAN "Enable UBSAN instrumentation" ON)

option(RAYCHELSCRIPT_BUILD_TOOLCHAIN "Build the entire RaychelScript toolchain" OFF)

option(RAYCHELSCRIPT_VM_ENABLE_DEBUG_TIMING "VM: enable logging execution time" OFF)
option(RAYCHELSCRIPT_VM_ENABLE_FP_EXCEPTION_STATE_DUMP "VM: dump state when a floating-point exception is thrown during execution" ON)

if(${RAYCHELSCRIPT_BUILD_TOOLCHAIN})
    set(RAYCHELSCRIPT_BUILD_LEXER ON)
    set(RAYCHELSCRIPT_BUILD_PARSER ON)
    set(RAYCHELSCRIPT_BUILD_OPTIMIZER ON)
    set(RAYCHELSCRIPT_BUILD_INTERPRETER ON)
    set(RAYCHELSCRIPT_BUILD_RASM_LIB ON)
    set(RAYCHELSCRIPT_BUILD_ASSEMBLER ON)
    set(RAYCHELSCRIPT_BUILD_VM ON)
endif()

if(${MSVC})
    if(${RAYCHELSCRIPT_ENABLE_UBSAN})
        message(STATUS "MSVC does not support UBSAN :(")
        set(RAYCHELSCRIPT_ENABLE_UBSAN OFF)
    endif()
endif()

if(${RAYCHELSCRIPT_ENABLE_ASAN})
    message(STATUS "Enabled ASAN instrumentation")
endif()

if(${RAYCHELSCRIPT_ENABLE_UBSAN})
    message(STATUS "Enabled UBSAN instrumentation")
endif()

include(cmake/find_dependencies.cmake)

add_subdirectory(shared)

if(${RAYCHELSCRIPT_BUILD_LEXER})
    message(STATUS "Adding RaychelScript Lexer...")
    add_subdirectory(Lexer)
endif()

if(${RAYCHELSCRIPT_BUILD_PARSER})
    message(STATUS "Adding RaychelScript Parser...")
    add_subdirectory(Parser)
endif()

if(${RAYCHELSCRIPT_BUILD_INTERPRETER})
    message(STATUS "Adding RaychelScript Interpreter...")
    add_subdirectory(Interpreter)
endif()

if(${RAYCHELSCRIPT_BUILD_RASM_LIB})
    message(STATUS "Adding RASM library...")
    add_subdirectory(rasm)
endif()

if(${RAYCHELSCRIPT_BUILD_ASSEMBLER})
    message(STATUS "Adding RaychelScript Assembler...")
    add_subdirectory(Assembler)
endif()

if(${RAYCHELSCRIPT_BUILD_VM})
    message(STATUS "Adding RaychelScript Virtual Machine...")
    add_subdirectory(VM)
endif()