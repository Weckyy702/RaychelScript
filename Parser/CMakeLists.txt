cmake_minimum_required(VERSION 3.14)

if(NOT ${RAYCHEL_LOGGER_EXTERNAL})
    find_package(RaychelLogger REQUIRED)
endif()

if(NOT ${RAYCHEL_CORE_EXTERNAL})
    find_package(RaychelCore REQUIRED)
endif()

set(RAYCHELSCRIPT_PARSER_INCLUDE_DIR
    "include/Parser/"
)

add_library(RaychelScriptParser SHARED
    "${RAYCHELSCRIPT_PARSER_INCLUDE_DIR}/Parser.h"
    "${RAYCHELSCRIPT_PARSER_INCLUDE_DIR}/ParsingContext.h"
    "${RAYCHELSCRIPT_PARSER_INCLUDE_DIR}/ParserPipe.h"
    "src/Parser.cpp"
    "src/ConfigBlock.cpp"
    "src/BodyBlock.cpp"
)

target_include_directories(RaychelScriptParser PUBLIC
    "include"
)

target_compile_features(RaychelScriptParser PUBLIC cxx_std_20)

target_compile_options(RaychelScriptParser PRIVATE ${RAYCHELSCRIPT_COMPILE_FLAGS})

target_link_libraries(RaychelScriptParser PUBLIC
    RaychelScriptBase
    RaychelScriptLexer
    RaychelLogger
)

target_link_options(RaychelScriptParser PUBLIC ${RAYCHELSCRIPT_LINK_FLAGS})

if(${RAYCHELSCRIPT_BUILD_TESTS})
    add_subdirectory(test)
endif()
