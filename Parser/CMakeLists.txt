find_package(RaychelCore REQUIRED)
find_package(RaychelLogger REQUIRED)

add_library(RaychelScriptParser SHARED
    "include/Parser.h"
    "src/Parser.cpp"
    "src/ConfigBlock.cpp"
    "src/BodyBlock.cpp"
)

target_include_directories(RaychelScriptParser PUBLIC
    "include"
)

target_compile_features(RaychelScriptParser PUBLIC cxx_std_20)

target_compile_options(RaychelScriptParser PUBLIC
    -Wall
    -Wextra
    -Wshadow
    -Wpedantic
    -Wconversion
    -Werror
)

target_link_libraries(RaychelScriptParser PUBLIC 
    RaychelScriptLexer
)

if(${RAYCHELSCRIPT_BUILD_TESTS})
    add_subdirectory(test)
endif()