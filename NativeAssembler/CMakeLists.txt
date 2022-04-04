cmake_minimum_required(VERSION 3.14)

if(NOT ${RAYCHEL_LOGGER_EXTERNAL})
    find_package(RaychelLogger REQUIRED)
endif()

if(NOT ${RAYCHEL_CORE_EXTERNAL})
    find_package(RaychelCore REQUIRED)
endif()

set(RAYCHELSCRIPT_NATIVE_ASSEMBLER_INCLUDE_DIR
    "include/NativeAssembler/"
)

add_library(RaychelScriptNativeAssembler SHARED
    "${RAYCHELSCRIPT_NATIVE_ASSEMBLER_INCLUDE_DIR}/NativeAssembler.h"
    "${RAYCHELSCRIPT_NATIVE_ASSEMBLER_INCLUDE_DIR}/NativeAssemblerErrorCode.h"

    "src/NativeAssembler.cpp"
)

target_include_directories(RaychelScriptNativeAssembler PUBLIC
    "include"
)

target_compile_features(RaychelScriptNativeAssembler PUBLIC cxx_std_20)

if(${MSVC})
    target_compile_options(RaychelScriptNativeAssembler PUBLIC
        /W4
    )
else()
    target_compile_options(RaychelScriptNativeAssembler PUBLIC
        -Wall
        -Wextra
        -Wshadow
        -Wpedantic
        -Wconversion
        -Werror
    )
    if(${RAYCHELSCRIPT_ENABLE_ASAN})
    target_compile_options(RaychelScriptNativeAssembler PUBLIC
        -fsanitize=address
    )
endif()
if(${RAYCHELSCRIPT_ENABLE_UBSAN})
    target_compile_options(RaychelScriptNativeAssembler PUBLIC
        -fsanitize=undefined
    )
endif()
endif()

target_link_libraries(RaychelScriptNativeAssembler PUBLIC 
    RaychelLogger
    RaychelScriptBase
    RaychelScriptAssembly
)

if(${RAYCHELSCRIPT_ENABLE_ASAN})
    target_link_options(RaychelScriptNativeAssembler PUBLIC
        -fsanitize=address
    )
endif()
if(${RAYCHELSCRIPT_ENABLE_UBSAN})
    target_link_options(RaychelScriptNativeAssembler PUBLIC
        -fsanitize=undefined
    )
endif()

if(${RAYCHELSCRIPT_BUILD_TESTS})
    add_subdirectory(test)
endif()
