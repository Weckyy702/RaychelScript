cmake_minimum_required(VERSION 3.14)

#LIBRARY
add_library(RaychelScriptBase INTERFACE)
target_include_directories(RaychelScriptBase INTERFACE
    "include"
)