project(vm C ASM)

add_library(${PROJECT_NAME} STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vm.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/instructions.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/instructions.c
)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

set(CMAKE_CONFIGURATION_TYPES "debug;release" CACHE STRING "" FORCE)
if (CMAKE_BUILD_TYPE STREQUAL "debug")
    message(STATUS "[VM] Debug build")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
elseif (CMAKE_BUILD_TYPE STREQUAL "release")
    message(STATUS "[VM] Release build")
    target_compile_definitions(${PROJECT_NAME} PRIVATE NDEBUG)
    target_compile_options(${PROJECT_NAME} PRIVATE -O3)
else()
    message(FATAL_ERROR "[VM] Unknown build type: ${CMAKE_BUILD_TYPE}")
endif()
