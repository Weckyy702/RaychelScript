cmake_minimum_required(VERSION 3.14)

if(NOT ${RAYCHEL_CORE_EXTERNAL})
find_package(RaychelCore REQUIRED)
find_package(RaychelLogger REQUIRED)
endif()

if(NOT ${RAYCHEL_MATH_EXTERNAL})
find_package(RaychelMath REQUIRED)
endif()

set(RAYCHELSCRIPT_VM_INCLUDE_DIR
    "include/VM/"
)

add_library(RaychelScriptVM INTERFACE
    "${RAYCHELSCRIPT_VM_INCLUDE_DIR}/VM.h"
    "${RAYCHELSCRIPT_VM_INCLUDE_DIR}/VMErrorCode.h"
    "${RAYCHELSCRIPT_VM_INCLUDE_DIR}/VMState.h"
    "${RAYCHELSCRIPT_VM_INCLUDE_DIR}/VMPipe.h"
    "${RAYCHELSCRIPT_VM_INCLUDE_DIR}/DumpState.h"
    "${RAYCHELSCRIPT_VM_INCLUDE_DIR}/Execute.h"
    "${RAYCHELSCRIPT_VM_INCLUDE_DIR}/InstructionHandlers.h"
)

target_include_directories(RaychelScriptVM INTERFACE
    "include"
)

target_compile_features(RaychelScriptVM INTERFACE cxx_std_20)

if(${MSVC})
    target_compile_options(RaychelScriptVM PUBLIC
        /W4
    )
else()
    target_compile_options(RaychelScriptVM INTERFACE
        -Wall
        -Wextra
        -Wshadow
        -Wpedantic
        -Wconversion
        -Werror
        -Wno-error=unknown-pragmas
    )
endif()

if(${RAYCHELSCRIPT_VM_ENABLE_DEBUG_TIMING})
    target_compile_definitions(RaychelScriptVM INTERFACE
        RAYCHELSCRIPT_VM_ENABLE_DEBUG_TIMING=1
    )
endif()

if(${RAYCHELSCRIPT_VM_ENABLE_FP_EXCEPTION_STATE_DUMP})
    target_compile_definitions(RaychelScriptVM INTERFACE
        RAYCHELSCRIPT_VM_ENABLE_FP_EXCEPTION_STATE_DUMP=1
    )
endif()

target_link_libraries(RaychelScriptVM INTERFACE
    RaychelLogger
    RaychelScriptBase
    RaychelMath
)

if(${RAYCHELSCRIPT_BUILD_TESTS})
    add_subdirectory(test)
endif()