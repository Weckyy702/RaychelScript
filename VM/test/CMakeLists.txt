find_package(RaychelLogger REQUIRED)

file(GLOB VM_TEST_SOURCE_FILES "*.test.cpp")

add_executable(VM_test
    ${VM_TEST_SOURCE_FILES}
)

target_compile_features(VM_test PUBLIC cxx_std_20)

if(${MSVC})
    target_compile_options(VM_test PUBLIC
        /W4
    )
else()
    target_compile_options(VM_test PUBLIC
        -Wall
        -Wextra
        -Wshadow
        -Wpedantic
        -Wconversion
        -Werror
    )
endif()

target_link_libraries(VM_test PUBLIC
    RaychelScriptBase
    RaychelScriptLexer
    RaychelScriptParser
    RaychelScriptAssembler
    RaychelScriptVM
    RaychelScriptInterpreter
    RaychelLogger
)