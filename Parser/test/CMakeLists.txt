find_package(RaychelLogger REQUIRED)

file(GLOB PARSER_TEST_SOURCE_FILES "*.test.cpp")

add_executable(Parser_test
    ${PARSER_TEST_SOURCE_FILES}
)

target_compile_features(Parser_test PUBLIC cxx_std_20)
target_compile_options(Parser_test PUBLIC
    -Wall
    -Wextra
    -Werror
    -Wpedantic
    -Wconversion
    -Wshadow
)

target_link_libraries(Parser_test PUBLIC
    RaychelScriptBase
    RaychelScriptParser
    RaychelLogger
)