find_package(RaychelLogger REQUIRED)
find_package(RaychelMath REQUIRED)
find_package(RaychelCore REQUIRED)

set(RAYCHELSCRIPT_INTERPRETER_INCLUDE_DIR
    "include/Interpreter"
)

add_library(RaychelScriptInterpreter SHARED
    "${RAYCHELSCRIPT_INTERPRETER_INCLUDE_DIR}/Concepts.h"
    "${RAYCHELSCRIPT_INTERPRETER_INCLUDE_DIR}/Interpreter.h"
    "${RAYCHELSCRIPT_INTERPRETER_INCLUDE_DIR}/InterpreterErrorCode.h"
    "${RAYCHELSCRIPT_INTERPRETER_INCLUDE_DIR}/InterpreterPipe.h"
    "${RAYCHELSCRIPT_INTERPRETER_INCLUDE_DIR}/InterpreterState.h"
    "${RAYCHELSCRIPT_INTERPRETER_INCLUDE_DIR}/StateFlags.h"
    
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
    if(${RAYCHELSCRIPT_ENABLE_ASAN})
        target_compile_options(RaychelScriptInterpreter PUBLIC
            -fsanitize=address
        )
    endif()
    if(${RAYCHELSCRIPT_ENABLE_UBSAN})
        target_compile_options(RaychelScriptInterpreter PUBLIC
            -fsanitize=undefined
        )
    endif()
endif()

target_link_libraries(RaychelScriptInterpreter PUBLIC
    RaychelMath
    RaychelScriptBase
    RaychelLogger
    RaychelCore
)

if(${RAYCHELSCRIPT_ENABLE_ASAN})
    target_link_options(RaychelScriptInterpreter PUBLIC
        -fsanitize=address
    )
endif()
if(${RAYCHELSCRIPT_ENABLE_UBSAN})
    target_link_options(RaychelScriptInterpreter PUBLIC
        -fsanitize=undefined
    )
endif()

if(${RAYCHELSCRIPT_BUILD_TESTS})
    add_subdirectory(test)
endif()