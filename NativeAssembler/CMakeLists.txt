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

target_compile_options(RaychelScriptNativeAssembler PRIVATE ${RAYCHELSCRIPT_COMPILE_FLAGS})

target_link_libraries(RaychelScriptNativeAssembler PUBLIC
    RaychelLogger
    RaychelScriptBase
    RaychelScriptAssembly
)

target_link_options(RaychelScriptNativeAssembler PUBLIC ${RAYCHELSCRIPT_LINK_FLAGS})

if(${RAYCHELSCRIPT_BUILD_TESTS})
    add_subdirectory(test)
endif()
