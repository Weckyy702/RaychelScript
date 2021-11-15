cmake_minimum_required(VERSION 3.14)

find_package(RaychelCore REQUIRED)
find_package(RaychelLogger REQUIRED)

#LIBRARY
add_library(RaychelScriptBase SHARED 
    "include/IndentHandler.h"
    "include/SourceLocation.h"
    "include/AST/AST.h"
    "include/AST/AST_Node.h"
    "include/AST/NodeData.h"
    "include/AST/NodeType.h"
    "include/Execution/ExecutionState.h"
    "include/Lexing/Alphabet.h"
    "include/Lexing/Token.h"
    "include/Lexing/TokenType.h"
    "src/IndentHandler.cpp"
)
target_include_directories(RaychelScriptBase PUBLIC
    "include"
)

target_compile_features(RaychelScriptBase PUBLIC cxx_std_20)

if(${MSVC})
    target_compile_options(RaychelScriptBase PUBLIC
        /W4
    )
else()
    target_compile_options(RaychelScriptBase PUBLIC
        -Wall
        -Wextra
        -Wshadow
        -Wpedantic
        -Wconversion
        -Werror
    )
endif()

target_link_libraries(RaychelScriptBase PUBLIC
    RaychelCore
    RaychelLogger
)