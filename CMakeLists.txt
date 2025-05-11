cmake_minimum_required(VERSION 3.15)
project(Axiom)

set_property(GLOBAL PROPERTY CMAKE_C_STANDARD 17)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory("anvil")
add_subdirectory("axiom")
