cmake_minimum_required(VERSION 3.14)

find_package(RaychelCore REQUIRED)
find_package(RaychelLogger REQUIRED)

add_library(RaychelScriptParser SHARED
    "include/Parser.h"
    "include/ParsingContext.h"
    "include/ParserPipe.h"
    "src/Parser.cpp"
    "src/ConfigBlock.cpp"
    "src/BodyBlock.cpp"
)

target_include_directories(RaychelScriptParser PUBLIC
    "include"
)

target_compile_features(RaychelScriptParser PUBLIC cxx_std_20)

if(${MSVC})
    target_compile_options(RaychelScriptParser PUBLIC
        /W4
    )
else()
    target_compile_options(RaychelScriptParser PUBLIC
        -Wall
        -Wextra
        -Wshadow
        -Wpedantic
        -Wconversion
        -Werror
    )
    if(${RAYCHELSCRIPT_ENABLE_ASAN})
        target_compile_options(RaychelScriptLexer PUBLIC
            -fsanitize=address
        )
    endif()
    if(${RAYCHELSCRIPT_ENABLE_UBSAN})
        target_compile_options(RaychelScriptLexer PUBLIC
            -fsanitize=undefined
        )
    endif()
endif()

target_link_libraries(RaychelScriptParser PUBLIC 
    RaychelScriptLexer
    RaychelLogger
)

if(${RAYCHELSCRIPT_ENABLE_ASAN})
    target_link_options(RaychelScriptLexer PUBLIC
        -fsanitize=address
    )
endif()
if(${RAYCHELSCRIPT_ENABLE_UBSAN})
    target_link_options(RaychelScriptLexer PUBLIC
        -fsanitize=undefined
    )
endif()

if(${RAYCHELSCRIPT_BUILD_TESTS})
    add_subdirectory(test)
endif()