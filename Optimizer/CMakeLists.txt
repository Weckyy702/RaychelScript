cmake_minimum_required(VERSION 3.14)

find_package(RaychelCore REQUIRED)
find_package(RaychelLogger REQUIRED)

set(RAYCHELSCRIPT_OPTIMIZER_INCLUDE_DIR 
    "include/Optimizer"
)

file(GLOB RAYCHELSCRIPT_OPTIMIZER_MODULE_HEADERS "${RAYCHELSCRIPT_OPTIMIZER_INCLUDE_DIR}modules/*.h")

add_library(RaychelScriptOptimizer SHARED
    "${RAYCHELSCRIPT_OPTIMIZER_INCLUDE_DIR}/Optimizer.h"
    "${RAYCHELSCRIPT_OPTIMIZER_INCLUDE_DIR}/OptimizerPipe.h"
    ${RAYCHELSCRIPT_OPTIMIZER_MODULE_HEADERS}
    "src/Optimizer.cpp"
)

target_include_directories(RaychelScriptOptimizer PUBLIC
    "include"
)

target_compile_features(RaychelScriptOptimizer PUBLIC cxx_std_20)

if(${MSVC})
    target_compile_options(RaychelScriptOptimizer PUBLIC
        /W4
    )
else()
    target_compile_options(RaychelScriptOptimizer PUBLIC
        -Wall
        -Wextra
        -Wshadow
        -Wpedantic
        -Wconversion
        -Werror
    )
    if(${RAYCHELSCRIPT_ENABLE_ASAN})
    target_compile_options(RaychelScriptOptimizer PUBLIC
        -fsanitize=address
    )
    endif()
    if(${RAYCHELSCRIPT_ENABLE_UBSAN})
        target_compile_options(RaychelScriptOptimizer PUBLIC
            -fsanitize=undefined
        )
    endif()
endif()

target_link_libraries(RaychelScriptOptimizer PUBLIC
    RaychelScriptBase
    RaychelLogger
)

if(${RAYCHELSCRIPT_ENABLE_ASAN})
    target_link_options(RaychelScriptOptimizer PUBLIC
        -fsanitize=address
    )
endif()
if(${RAYCHELSCRIPT_ENABLE_UBSAN})
    target_link_options(RaychelScriptOptimizer PUBLIC
        -fsanitize=undefined
    )
endif()

if(${RAYCHELSCRIPT_BUILD_TESTS})
    add_subdirectory(test)
endif()