find_package(RaychelLogger REQUIRED)

file(GLOB PARSER_TEST_SOURCE_FILES "*.test.cpp")

add_executable(Parser_test
    ${PARSER_TEST_SOURCE_FILES}
)

target_compile_features(Parser_test PUBLIC cxx_std_20)

if(${MSVC})
    target_compile_options(Parser_test PUBLIC
        /W4
    )
else()
    target_compile_options(Parser_test PUBLIC
        -Wall
        -Wextra
        -Wshadow
        -Wpedantic
        -Wconversion
        -Werror
    )
endif()

target_link_libraries(Parser_test PUBLIC
    RaychelScriptBase
    RaychelScriptParser
    RaychelLogger
)