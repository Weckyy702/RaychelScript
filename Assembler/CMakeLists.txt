cmake_minimum_required(VERSION 3.14)

find_package(RaychelCore REQUIRED)
find_package(RaychelLogger REQUIRED)

set(RAYCHELSCRIPT_ASSEMBLER_INCLUDE_DIR
    "include/Assembler/"
)

add_library(RaychelScriptAssembler SHARED
    "${RAYCHELSCRIPT_ASSEMBLER_INCLUDE_DIR}/Assembler.h"
    "${RAYCHELSCRIPT_ASSEMBLER_INCLUDE_DIR}/AssemblingContext.h"
    "${RAYCHELSCRIPT_ASSEMBLER_INCLUDE_DIR}/AssemblerPipe.h"

    "src/Assembler.cpp"
)

target_include_directories(RaychelScriptAssembler PUBLIC
    "include"
)

target_compile_features(RaychelScriptAssembler PUBLIC cxx_std_20)

if(${MSVC})
    target_compile_options(RaychelScriptAssembler PUBLIC
        /W4
    )
else()
    target_compile_options(RaychelScriptAssembler PUBLIC
        -Wall
        -Wextra
        -Wshadow
        -Wpedantic
        -Wconversion
        -Werror
    )
    if(${RAYCHELSCRIPT_ENABLE_ASAN})
    target_compile_options(RaychelScriptAssembler PUBLIC
        -fsanitize=address
    )
endif()
if(${RAYCHELSCRIPT_ENABLE_UBSAN})
    target_compile_options(RaychelScriptAssembler PUBLIC
        -fsanitize=undefined
    )
endif()
endif()

target_link_libraries(RaychelScriptAssembler PUBLIC 
    RaychelLogger
    RaychelScriptBase
    RaychelScriptAssembly
)

if(${RAYCHELSCRIPT_ENABLE_ASAN})
    target_link_options(RaychelScriptAssembler PUBLIC
        -fsanitize=address
    )
endif()
if(${RAYCHELSCRIPT_ENABLE_UBSAN})
    target_link_options(RaychelScriptAssembler PUBLIC
        -fsanitize=undefined
    )
endif()

if(${RAYCHELSCRIPT_BUILD_TESTS})
    add_subdirectory(test)
endif()