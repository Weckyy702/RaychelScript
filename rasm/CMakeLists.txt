cmake_minimum_required(VERSION 3.14)

find_package(RaychelCore REQUIRED)
find_package(RaychelLogger REQUIRED)

set(RAYCHELSCRIPT_ASSEMBLY_INCLUDE_DIR
    "include/rasm/"
)

add_library(RaychelScriptAssembly INTERFACE
    "${RAYCHELSCRIPT_ASSEMBLY_INCLUDE_DIR}/Instruction.h"
    "${RAYCHELSCRIPT_ASSEMBLY_INCLUDE_DIR}/OpCode.h"
)

target_include_directories(RaychelScriptAssembly INTERFACE
    "include"
)

target_compile_features(RaychelScriptAssembly INTERFACE cxx_std_20)

if(${MSVC})
    target_compile_options(RaychelScriptAssembly INTERFACE
        /W4
    )
else()
    target_compile_options(RaychelScriptAssembly INTERFACE
        -Wall
        -Wextra
        -Wshadow
        -Wpedantic
        -Wconversion
        -Werror
    )
endif()

target_link_libraries(RaychelScriptAssembly INTERFACE 
    RaychelLogger
    RaychelScriptBase
)

if(${RAYCHELSCRIPT_BUILD_TESTS})
    add_subdirectory(test)
endif()