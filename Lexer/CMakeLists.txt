find_package(RaychelCore REQUIRED)
find_package(RaychelLogger REQUIRED)

add_library(RaychelScriptLexer SHARED
    "include/Lexer.h"
    "include/LexResult.h"
    "include/LexerPipe.h"
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
endif()

target_link_libraries(RaychelScriptLexer PUBLIC 
    RaychelScriptBase
    RaychelLogger
)

if(${RAYCHELSCRIPT_BUILD_TESTS})
    add_subdirectory(test)
endif()