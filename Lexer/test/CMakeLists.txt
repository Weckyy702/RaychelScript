if(NOT ${RAYCHEL_CORE_EXTERNAL})
    find_package(RaychelLogger REQUIRED)
endif()

file(GLOB LEXER_TEST_SOURCE_FILES "*.test.cpp")

add_executable(Lexer_test
    ${LEXER_TEST_SOURCE_FILES}
)

target_compile_features(Lexer_test PUBLIC cxx_std_20)

if(${MSVC})
    target_compile_options(Lexer_test PUBLIC
        /W4
    )
else()
    target_compile_options(Lexer_test PUBLIC
        -Wall
        -Wextra
        -Wshadow
        -Wpedantic
        -Wconversion
        -Werror
    )
endif()

target_link_libraries(Lexer_test PUBLIC
    RaychelScriptBase
    RaychelScriptLexer
    RaychelLogger
)