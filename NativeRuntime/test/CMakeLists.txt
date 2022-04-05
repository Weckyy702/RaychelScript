if(NOT ${RAYCHEL_LOGGER_EXTERNAL})
    find_package(RaychelLogger REQUIRED)
endif()

file(GLOB RAYCHELSCRIPT_RUNTIME_TEST_SOURCES "*.test.cpp")

add_executable(RaychelScriptRuntime_test
    ${RAYCHELSCRIPT_RUNTIME_TEST_SOURCES}
)

target_compile_features(RaychelScriptRuntime_test PUBLIC cxx_std_20)

if(${MSVC})
    target_compile_options(RaychelScriptRuntime_test PUBLIC
        /W4
    )
else()
    target_compile_options(RaychelScriptRuntime_test PUBLIC
        -Wall
        -Wextra
        -Wshadow
        -Wpedantic
        -Wconversion
        -Werror
    )
endif()

target_link_libraries(RaychelScriptRuntime_test PUBLIC
    RaychelScriptRuntime
    RaychelLogger
)
