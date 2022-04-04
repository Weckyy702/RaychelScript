if(NOT ${RAYCHEL_LOGGER_EXTERNAL})
    find_package(RaychelLogger REQUIRED)
endif()

file(GLOB NATIVE_ASSEMBLER_TEST_SOURCES "*.test.cpp")

add_executable(NativeAssembler_test
    ${NATIVE_ASSEMBLER_TEST_SOURCES}
)

target_compile_features(NativeAssembler_test PUBLIC cxx_std_20)

if(${MSVC})
    target_compile_options(NativeAssembler_test PUBLIC
        /W4
    )
else()
    target_compile_options(NativeAssembler_test PUBLIC
        -Wall
        -Wextra
        -Wshadow
        -Wpedantic
        -Wconversion
        -Werror
    )
endif()

target_link_libraries(NativeAssembler_test PUBLIC
    RaychelScriptBase
    RaychelScriptLexer
    RaychelScriptParser
    RaychelScriptAssembler
    RaychelScriptNativeAssembler
    RaychelLogger
)
