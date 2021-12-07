cmake_minimum_required(VERSION 3.14)

find_package(RaychelCore REQUIRED)
find_package(RaychelLogger REQUIRED)

file(GLOB RAYCHELSCRIPT_OPTIMIZER_MODULE_HEADERS "include/modules/*.h")

add_library(RaychelScriptOptimizer SHARED
    "include/Optimizer.h"
    "include/OptimizerPipe.h"
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
endif()

target_link_libraries(RaychelScriptOptimizer PUBLIC
    RaychelScriptBase
    RaychelScriptParser
    RaychelScriptInterpreter
    RaychelLogger
)

if(${RAYCHELSCRIPT_BUILD_TESTS})
    add_subdirectory(test)
endif()