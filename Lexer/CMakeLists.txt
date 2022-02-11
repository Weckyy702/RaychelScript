if(NOT ${RAYCHEL_CORE_EXTERNAL})
    find_package(RaychelCore REQUIRED)
    find_package(RaychelLogger REQUIRED)
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

if(${MSVC})
    target_compile_options(RaychelScriptLexer PUBLIC
        /W4
    )
else()
    target_compile_options(RaychelScriptLexer PUBLIC
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

target_link_libraries(RaychelScriptLexer PUBLIC 
    RaychelScriptBase
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