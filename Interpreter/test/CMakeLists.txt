find_package(RaychelLogger REQUIRED)

file(GLOB INTERPRETER_TEST_SOURCE_FILES "*.test.cpp")

add_executable(Interpreter_test
    ${INTERPRETER_TEST_SOURCE_FILES}
)

target_compile_features(Interpreter_test PUBLIC cxx_std_20)

if(${MSVC})
    target_compile_options(Interpreter_test PUBLIC
        /W4
    )
else()
    target_compile_options(Interpreter_test PUBLIC
        -Wall
        -Wextra
        -Wshadow
        -Wpedantic
        -Wconversion
        -Werror
    )
endif()

target_link_libraries(Interpreter_test PUBLIC
    RaychelScriptBase
    RaychelScriptInterpreter
    RaychelScriptParser
    RaychelLogger
)

if(NOT ${MSVC})
    target_link_libraries(Interpreter_test PUBLIC
        pthread
    )
endif()