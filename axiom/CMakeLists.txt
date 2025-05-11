project(axiom C ASM)

file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
)

file(GLOB HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(${PROJECT_NAME}
    anvil
)

set(CMAKE_CONFIGURATION_TYPES "debug;release" CACHE STRING "" FORCE)
if (CMAKE_BUILD_TYPE STREQUAL "debug")
    message(STATUS "[Axiom] Debug build")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
elseif (CMAKE_BUILD_TYPE STREQUAL "release")
    message(STATUS "[Axiom] Release build")
    target_compile_definitions(${PROJECT_NAME} PRIVATE NDEBUG)
    target_compile_options(${PROJECT_NAME} PRIVATE -O3)
else()
    message(FATAL_ERROR "[Axiom] unknown build type: ${CMAKE_BUILD_TYPE}")
endif()
