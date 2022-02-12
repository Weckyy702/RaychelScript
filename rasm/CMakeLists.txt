cmake_minimum_required(VERSION 3.14)

if(NOT ${RAYCHEL_LOGGER_EXTERNAL})
    find_package(RaychelLogger REQUIRED)
endif()

if(NOT ${RAYCHEL_CORE_EXTERNAL})
find_package(RaychelCore REQUIRED)
endif()

set(RAYCHELSCRIPT_ASSEMBLY_INCLUDE_DIR
    "include/rasm/"
)

add_library(RaychelScriptAssembly SHARED
    "${RAYCHELSCRIPT_ASSEMBLY_INCLUDE_DIR}/magic.h"
    "${RAYCHELSCRIPT_ASSEMBLY_INCLUDE_DIR}/ReadPipe.h"
    "${RAYCHELSCRIPT_ASSEMBLY_INCLUDE_DIR}/WritePipe.h"


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
    if(${RAYCHELSCRIPT_ENABLE_ASAN})
    target_compile_options(RaychelScriptAssembly PUBLIC
        -fsanitize=address
    )
endif()
if(${RAYCHELSCRIPT_ENABLE_UBSAN})
    target_compile_options(RaychelScriptAssembly PUBLIC
        -fsanitize=undefined
    )
endif()
endif()

target_link_libraries(RaychelScriptAssembly PUBLIC 
    RaychelLogger
    RaychelScriptBase
)

if(${RAYCHELSCRIPT_ENABLE_ASAN})
    target_link_options(RaychelScriptAssembly PUBLIC
        -fsanitize=address
    )
endif()
if(${RAYCHELSCRIPT_ENABLE_UBSAN})
    target_link_options(RaychelScriptAssembly PUBLIC
        -fsanitize=undefined
    )
endif()

if(${RAYCHELSCRIPT_BUILD_TESTS})
    add_subdirectory(test)
endif()