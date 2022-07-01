cmake_minimum_required(VERSION 3.14)

if(NOT ${RAYCHEL_LOGGER_EXTERNAL})
    find_package(RaychelLogger REQUIRED)
endif()

if(NOT ${RAYCHEL_CORE_EXTERNAL})
find_package(RaychelCore REQUIRED)
endif()

set(RAYCHELSCRIPT_ASSEMBLER_INCLUDE_DIR
    "include/Assembler/"
)

add_library(RaychelScriptAssembler SHARED
    "${RAYCHELSCRIPT_ASSEMBLER_INCLUDE_DIR}/Assembler.h"
    "${RAYCHELSCRIPT_ASSEMBLER_INCLUDE_DIR}/AssemblingContext.h"
    "${RAYCHELSCRIPT_ASSEMBLER_INCLUDE_DIR}/AssemblerPipe.h"

    "src/Assembler.cpp"
)

target_include_directories(RaychelScriptAssembler PUBLIC
    "include"
)

target_compile_features(RaychelScriptAssembler PUBLIC cxx_std_20)

target_compile_options(RaychelScriptAssembler PRIVATE ${RAYCHELSCRIPT_COMPILE_FLAGS})

target_link_libraries(RaychelScriptAssembler PUBLIC
    RaychelLogger
    RaychelScriptBase
)

target_link_options(RaychelScriptAssembler PUBLIC ${RAYCHELSCRIPT_LINK_FLAGS})

if(${RAYCHELSCRIPT_BUILD_TESTS})
    add_subdirectory(test)
endif()
