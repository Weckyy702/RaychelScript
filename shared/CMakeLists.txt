cmake_minimum_required(VERSION 3.14)

if(NOT ${RAYCHEL_LOGGER_EXTERNAL})
    find_package(RaychelLogger REQUIRED)
endif()

if(NOT ${RAYCHEL_CORE_EXTERNAL})
    find_package(RaychelCore REQUIRED)
endif()

set(RAYCHELSCRIPT_BASE_INCLUDE_DIR "include/shared")

#LIBRARY
add_library(RaychelScriptBase INTERFACE
    "${RAYCHELSCRIPT_BASE_INCLUDE_DIR}/IndentHandler.h"
    "${RAYCHELSCRIPT_BASE_INCLUDE_DIR}/SourceLocation.h"

    "${RAYCHELSCRIPT_BASE_INCLUDE_DIR}/AST/AST.h"
    "${RAYCHELSCRIPT_BASE_INCLUDE_DIR}/AST/AST_Node.h"
    "${RAYCHELSCRIPT_BASE_INCLUDE_DIR}/AST/ConfigBlock.h"
    "${RAYCHELSCRIPT_BASE_INCLUDE_DIR}/AST/FunctionData.h"
    "${RAYCHELSCRIPT_BASE_INCLUDE_DIR}/AST/NodeData.h"
    "${RAYCHELSCRIPT_BASE_INCLUDE_DIR}/AST/NodeHasValue.h"
    "${RAYCHELSCRIPT_BASE_INCLUDE_DIR}/AST/NodeType.h"

    "${RAYCHELSCRIPT_BASE_INCLUDE_DIR}/Lexing/Alphabet.h"
    "${RAYCHELSCRIPT_BASE_INCLUDE_DIR}/Lexing/Token.h"
    "${RAYCHELSCRIPT_BASE_INCLUDE_DIR}/Lexing/TokenType.h"

    "${RAYCHELSCRIPT_BASE_INCLUDE_DIR}/Misc/PrintAST.h"
    "${RAYCHELSCRIPT_BASE_INCLUDE_DIR}/Misc/WalkAST.h"

    "${RAYCHELSCRIPT_BASE_INCLUDE_DIR}/rasm/OpCode.h"
    "${RAYCHELSCRIPT_BASE_INCLUDE_DIR}/rasm/Instruction.h"

    "${RAYCHELSCRIPT_BASE_INCLUDE_DIR}/VM/VMData.h"
)
target_include_directories(RaychelScriptBase INTERFACE
    "include"
)

target_compile_features(RaychelScriptBase INTERFACE cxx_std_20)

target_link_libraries(RaychelScriptBase INTERFACE
    RaychelCore
    RaychelLogger
)
