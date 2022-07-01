if(NOT ${RAYCHEL_LOGGER_EXTERNAL})
    find_package(RaychelLogger REQUIRED)
endif()

if(NOT ${RAYCHEL_CORE_EXTERNAL})
find_package(RaychelLogger REQUIRED)
endif()

if(NOT ${RAYCHEL_MATH_EXTERNAL})
    find_package(RaychelMath REQUIRED)
endif()

set(RAYCHELSCRIPT_INTERPRETER_INCLUDE_DIR
    "include/Interpreter"
)

add_library(RaychelScriptInterpreter SHARED
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

target_compile_options(RaychelScriptInterpreter PRIVATE ${RAYCHELSCRIPT_COMPILE_FLAGS})

target_link_libraries(RaychelScriptInterpreter PUBLIC
    RaychelMath
    RaychelScriptBase
    RaychelLogger
    RaychelCore
)

target_link_options(RaychelScriptInterpreter PUBLIC ${RAYCHELSCRIPT_LINK_FLAGS})

if(${RAYCHELSCRIPT_BUILD_TESTS})
    add_subdirectory(test)
endif()
