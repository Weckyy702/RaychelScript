cmake_minimum_required(VERSION 3.14)

find_package(RaychelCore REQUIRED)
find_package(RaychelLogger REQUIRED)

set(RAYCHELSCRIPT_ASSEMBLY_INCLUDE_DIR
    "include/rasm/"
)

add_library(RaychelScriptAssembly SHARED
    "${RAYCHELSCRIPT_ASSEMBLY_INCLUDE_DIR}/Instruction.h"
    "${RAYCHELSCRIPT_ASSEMBLY_INCLUDE_DIR}/OpCode.h"
    "${RAYCHELSCRIPT_ASSEMBLY_INCLUDE_DIR}/InstructionData.h"
    "${RAYCHELSCRIPT_ASSEMBLY_INCLUDE_DIR}/VMData.h"
    "${RAYCHELSCRIPT_ASSEMBLY_INCLUDE_DIR}/magic.h"

    "src/read.cpp"
    "src/write.cpp"
    "src/magic.cpp"
)

target_include_directories(RaychelScriptAssembly PUBLIC
    "include"
)

target_compile_features(RaychelScriptAssembly PUBLIC cxx_std_20)

if(${MSVC})
    target_compile_options(RaychelScriptAssembly PUBLIC
        /W4
    )
else()
    target_compile_options(RaychelScriptAssembly PUBLIC
        -Wall
        -Wextra
        -Wshadow
        -Wpedantic
        -Wconversion
        -Werror
    )
endif()

target_link_libraries(RaychelScriptAssembly PUBLIC 
    RaychelLogger
    RaychelScriptBase
)

if(${RAYCHELSCRIPT_BUILD_TESTS})
    add_subdirectory(test)
endif()