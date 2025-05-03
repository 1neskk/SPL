cmake_minimum_required(VERSION 3.15)
project(SPL)

set_property(GLOBAL PROPERTY CMAKE_C_STANDARD 17)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory("vm")
add_subdirectory("compiler")
