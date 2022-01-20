cmake_minimum_required(VERSION 3.14)

find_package(RaychelCore REQUIRED)
find_package(RaychelLogger REQUIRED)

set(RAYCHELSCRIPT_VM_INCLUDE_DIR
    "include/VM/"
)

add_library(RaychelScriptVM SHARED
    "${RAYCHELSCRIPT_VM_INCLUDE_DIR}/VM.h"
    "${RAYCHELSCRIPT_VM_INCLUDE_DIR}/VMErrorCode.h"
    "${RAYCHELSCRIPT_VM_INCLUDE_DIR}/VMState.h"
    "${RAYCHELSCRIPT_VM_INCLUDE_DIR}/VMPipe.h"
    

    "src/VM.cpp"
)

target_include_directories(RaychelScriptVM PUBLIC
    "include"
)

target_compile_features(RaychelScriptVM PUBLIC cxx_std_20)

if(${MSVC})
    target_compile_options(RaychelScriptVM PUBLIC
        /W4
    )
else()
    target_compile_options(RaychelScriptVM PUBLIC
        -Wall
        -Wextra
        -Wshadow
        -Wpedantic
        -Wconversion
        -Werror
    )
    if(${RAYCHELSCRIPT_ENABLE_ASAN})
    target_compile_options(RaychelScriptVM PUBLIC
        -fsanitize=address
    )
endif()
if(${RAYCHELSCRIPT_ENABLE_UBSAN})
    target_compile_options(RaychelScriptVM PUBLIC
        -fsanitize=undefined
    )
endif()
endif()

target_link_libraries(RaychelScriptVM PUBLIC 
    RaychelLogger
    RaychelScriptBase
)

if(${RAYCHELSCRIPT_ENABLE_ASAN})
    target_link_options(RaychelScriptVM PUBLIC
        -fsanitize=address
    )
endif()
if(${RAYCHELSCRIPT_ENABLE_UBSAN})
    target_link_options(RaychelScriptVM PUBLIC
        -fsanitize=undefined
    )
endif()

if(${RAYCHELSCRIPT_BUILD_TESTS})
    add_subdirectory(test)
endif()