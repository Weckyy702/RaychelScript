if(NOT ${RAYCHEL_LOGGER_EXTERNAL})
    find_package(RaychelLogger REQUIRED)
endif()

if(NOT ${RAYCHEL_CORE_EXTERNAL})
    find_package(RaychelCore REQUIRED)
endif()

set(RAYCHELSCRIPT_LEXER_INCLUDE_DIR
    "include/Lexer"
)

add_library(RaychelScriptLexer SHARED
    "${RAYCHELSCRIPT_LEXER_INCLUDE_DIR}/Lexer.h"
    "${RAYCHELSCRIPT_LEXER_INCLUDE_DIR}/LexResult.h"
    "${RAYCHELSCRIPT_LEXER_INCLUDE_DIR}/LexerPipe.h"

    "src/Lexer.cpp"
)

target_include_directories(RaychelScriptLexer PUBLIC
    "include"
)

target_compile_features(RaychelScriptLexer PUBLIC cxx_std_20)

target_compile_options(RaychelScriptLexer PRIVATE ${RAYCHELSCRIPT_COMPILE_FLAGS})

target_link_libraries(RaychelScriptLexer PUBLIC
    RaychelScriptBase
    RaychelLogger
)

target_link_options(RaychelScriptLexer PUBLIC ${RAYCHELSCRIPT_LINK_FLAGS})

if(${RAYCHELSCRIPT_BUILD_TESTS})
    add_subdirectory(test)
endif()
