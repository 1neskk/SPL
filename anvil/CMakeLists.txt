project(anvil C ASM)

add_library(${PROJECT_NAME} STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vm.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/instructions.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/memory.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/io.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/assembler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/parser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/error.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/instructions.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/memory.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/memory.s
    ${CMAKE_CURRENT_SOURCE_DIR}/src/io.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/io.s
    ${CMAKE_CURRENT_SOURCE_DIR}/src/assembler.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/parser.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/error.c
)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

set(CMAKE_CONFIGURATION_TYPES "debug;release;test" CACHE STRING "" FORCE)
if (CMAKE_BUILD_TYPE STREQUAL "debug")
    message(STATUS "[ANVIL] Debug build")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
elseif (CMAKE_BUILD_TYPE STREQUAL "release")
    message(STATUS "[ANVIL] Release build")
    target_compile_definitions(${PROJECT_NAME} PRIVATE NDEBUG)
    target_compile_options(${PROJECT_NAME} PRIVATE -O3)
elseif (CMAKE_BUILD_TYPE STREQUAL "test")
    message(STATUS "[ANVIL] Test build")
    add_executable(${PROJECT_NAME}_test
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_vm.c
    )
    target_link_libraries(${PROJECT_NAME}_test ${PROJECT_NAME})
    target_compile_definitions(${PROJECT_NAME}_test PRIVATE TEST)
    target_compile_options(${PROJECT_NAME}_test PRIVATE -O3)
else()
    message(FATAL_ERROR "[ANVIL] Unknown build type: ${CMAKE_BUILD_TYPE}")
endif()
