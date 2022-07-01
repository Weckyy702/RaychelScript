cmake_minimum_required(VERSION 3.14)

if(NOT ${RAYCHEL_LOGGER_EXTERNAL})
    find_package(RaychelLogger REQUIRED)
endif()

if(NOT ${RAYCHEL_CORE_EXTERNAL})
    find_package(RaychelCore REQUIRED)
endif()

set(RAYCHELSCRIPT_RUNTIME_INCLUDE_DIR
    "include/NativeRuntime/"
)

add_library(RaychelScriptRuntime SHARED
    "${RAYCHELSCRIPT_RUNTIME_INCLUDE_DIR}/ScriptRunner.h"
    "${RAYCHELSCRIPT_RUNTIME_INCLUDE_DIR}/RuntimeErrorCode.h"

    "src/ScriptRunner.cpp"
)

target_include_directories(RaychelScriptRuntime PUBLIC
    "include"
)

target_compile_features(RaychelScriptRuntime PUBLIC cxx_std_20)

target_compile_options(RaychelScriptRuntime PRIVATE ${RAYCHELSCRIPT_COMPILE_FLAGS})

target_link_libraries(RaychelScriptRuntime PUBLIC
    RaychelLogger
    RaychelScriptBase
)

target_link_options(RaychelScriptRuntime PUBLIC ${RAYCHELSCRIPT_LINK_FLAGS})

if(${RAYCHELSCRIPT_BUILD_TESTS})
    add_subdirectory(test)
endif()
