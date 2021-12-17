find_package(RaychelLogger REQUIRED)

file(GLOB RASM_TEST_SOURCE_FILES "*.test.cpp")

add_executable(Rasm_test
    ${RASM_TEST_SOURCE_FILES}
)

target_compile_features(Rasm_test PUBLIC cxx_std_20)

if(${MSVC})
    target_compile_options(Rasm_test PUBLIC
        /W4
    )
else()
    target_compile_options(Rasm_test PUBLIC
        -Wall
        -Wextra
        -Wshadow
        -Wpedantic
        -Wconversion
        -Werror
    )
endif()

target_link_libraries(Rasm_test PUBLIC
    RaychelScriptBase
    RaychelScriptAssembly
    RaychelLogger
)