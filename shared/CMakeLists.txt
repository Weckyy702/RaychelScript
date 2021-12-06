cmake_minimum_required(VERSION 3.14)

find_package(RaychelCore REQUIRED)
find_package(RaychelLogger REQUIRED)

#LIBRARY
add_library(RaychelScriptBase INTERFACE
    "include/IndentHandler.h"
    "include/SourceLocation.h"

    "include/AST/AST.h"
    "include/AST/AST_Node.h"
    "include/AST/ConfigBlock.h"
    "include/AST/NodeData.h"
    "include/AST/NodeHasValue.h"
    "include/AST/NodeType.h"

    "include/Execution/ConstantDescriptor.h"
    "include/Execution/DescriptorID.h"
    "include/Execution/VariableDescriptor.h"

    "include/Lexing/Alphabet.h"
    "include/Lexing/Token.h"
    "include/Lexing/TokenType.h"

    "include/Misc/PrintAST.h"
    "include/Misc/WalkAST.h"
)
target_include_directories(RaychelScriptBase INTERFACE
    "include"
)

target_compile_features(RaychelScriptBase INTERFACE cxx_std_20)

if(${MSVC})
    target_compile_options(RaychelScriptBase INTERFACE
        /W4
    )
else()
    target_compile_options(RaychelScriptBase INTERFACE
        -Wall
        -Wextra
        -Wshadow
        -Wpedantic
        -Wconversion
        -Werror
    )
endif()

target_link_libraries(RaychelScriptBase INTERFACE
    RaychelCore
    RaychelLogger
)