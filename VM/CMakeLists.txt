cmake_minimum_required(VERSION 3.14)

if(NOT ${RAYCHEL_LOGGER_EXTERNAL})
    find_package(RaychelLogger REQUIRED)
endif()

if(NOT ${RAYCHEL_CORE_EXTERNAL})
find_package(RaychelCore REQUIRED)
endif()

if(NOT ${RAYCHEL_MATH_EXTERNAL})
find_package(RaychelMath REQUIRED)
endif()

set(RAYCHELSCRIPT_VM_INCLUDE_DIR
    "include/VM/"
)

add_library(RaychelScriptVM SHARED
    "${RAYCHELSCRIPT_VM_INCLUDE_DIR}/VM.h"
    "${RAYCHELSCRIPT_VM_INCLUDE_DIR}/VMErrorCode.h"
    "${RAYCHELSCRIPT_VM_INCLUDE_DIR}/VMState.h"
    "${RAYCHELSCRIPT_VM_INCLUDE_DIR}/VMPipe.h"

    "src/VM.cpp"
    "src/VMState.cpp"
)

target_include_directories(RaychelScriptVM PUBLIC
    "include"
)

target_compile_features(RaychelScriptVM PUBLIC cxx_std_20)

target_compile_options(RaychelScriptVM PRIVATE
    ${RAYCHELSCRIPT_COMPILE_FLAGS}
    -Wno-error=unknown-pragmas
)

target_link_options(RaychelScriptVM PUBLIC ${RAYCHELSCRIPT_LINK_FLAGS})

target_link_libraries(RaychelScriptVM PUBLIC
    RaychelLogger
    RaychelScriptBase
    RaychelMath
)

if(${RAYCHELSCRIPT_BUILD_TESTS})
    add_subdirectory(test)
endif()
