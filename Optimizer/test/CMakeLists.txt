find_package(RaychelLogger REQUIRED)

file(GLOB OPTIMIZER_TEST_SOURCE_FILES "*.test.cpp")

add_executable(Optimizer_test
    ${OPTIMIZER_TEST_SOURCE_FILES}
)

target_compile_features(Optimizer_test PUBLIC cxx_std_20)

if(${MSVC})
    target_compile_options(Optimizer_test PUBLIC
        /W4
    )
else()
    target_compile_options(Optimizer_test PUBLIC
        -Wall
        -Wextra
        -Wshadow
        -Wpedantic
        -Wconversion
        -Werror
    )
endif()

target_link_libraries(Optimizer_test PUBLIC
    RaychelScriptBase
    RaychelScriptParser
    RaychelScriptOptimizer
    RaychelScriptInterpreter
    RaychelLogger
)