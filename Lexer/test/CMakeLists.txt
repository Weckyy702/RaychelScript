find_package(RaychelLogger REQUIRED)

file(GLOB LEXER_TEST_SOURCE_FILES "*.test.cpp")

add_executable(Lexer_test
    ${LEXER_TEST_SOURCE_FILES}
)

target_compile_features(Lexer_test PUBLIC cxx_std_20)
target_compile_options(Lexer_test PUBLIC
    -Wall
    -Wextra
    -Werror
    -Wpedantic
    -Wconversion
    -Wshadow
)

target_link_libraries(Lexer_test PUBLIC
    RaychelScriptBase
    RaychelScriptParser
    RaychelLogger
)