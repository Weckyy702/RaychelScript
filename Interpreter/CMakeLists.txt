find_package(RaychelLogger REQUIRED)

add_library(RaychelScriptInterpreter SHARED
    "include/ExecutionErrorCode.h"
    "include/ExecutionState.h"
    "include/Interpreter.h"
    "src/Interpreter.cpp"
)

target_include_directories(RaychelScriptInterpreter PUBLIC
    "include"
)

target_compile_features(RaychelScriptInterpreter PUBLIC cxx_std_20)

if(${MSVC})
    target_compile_options(RaychelScriptInterpreter PUBLIC
        /W4
    )
else()
    target_compile_options(RaychelScriptInterpreter PUBLIC
        -Wall
        -Wextra
        -Wshadow
        -Wpedantic
        -Wconversion
        -Werror
    )
endif()

target_link_libraries(RaychelScriptInterpreter PUBLIC
    RaychelScriptParser
    RaychelLogger
)

if(${RAYCHELSCRIPT_BUILD_TESTS})
    add_subdirectory(test)
endif()