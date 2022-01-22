find_package(RaychelLogger REQUIRED)

file(GLOB ASSEMBLER_TEST_SOURCE_FILES "*.test.cpp")

add_executable(Assembler_test
    ${ASSEMBLER_TEST_SOURCE_FILES}
)

target_compile_features(Assembler_test PUBLIC cxx_std_20)

if(${MSVC})
    target_compile_options(Assembler_test PUBLIC
        /W4
    )
else()
    target_compile_options(Assembler_test PUBLIC
        -Wall
        -Wextra
        -Wshadow
        -Wpedantic
        -Wconversion
        -Werror
    )
endif()

target_link_libraries(Assembler_test PUBLIC
    RaychelScriptBase
    RaychelScriptLexer
    RaychelScriptParser
    RaychelScriptAssembler
    RaychelScriptAssembly
    RaychelLogger
)