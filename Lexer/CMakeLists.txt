find_package(RaychelCore REQUIRED)
find_package(RaychelLogger REQUIRED)

add_library(RaychelScriptLexer SHARED
    "include/TokenType.h"
    "include/Token.h"
    "include/Lexer.h"
    "src/Lexer.cpp"
)

target_include_directories(RaychelScriptLexer PUBLIC
    "include"
)

target_compile_features(RaychelScriptLexer PUBLIC cxx_std_20)

target_compile_options(RaychelScriptLexer PUBLIC
    -Wall
    -Wextra
    -Wshadow
    -Wpedantic
    -Wconversion
    -Werror
)

target_link_libraries(RaychelScriptLexer PUBLIC 
    RaychelScriptBase
    RaychelCore
)

if(${RAYCHELSCRIPT_BUILD_TESTS})
    add_subdirectory(test)
endif()