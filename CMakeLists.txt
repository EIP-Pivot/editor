cmake_minimum_required(VERSION 3.20)
project(user-interface)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "Looking for lld")
find_program(LLD_FOUND lld)
if (LLD_FOUND)
    message(STATUS "Looking for lld - found")
    set(CMAKE_C_LINKER_FLAGS "-fuse-ld=lld")
    set(CMAKE_CXX_LINKER_FLAGS "-fuse-ld=lld")
    set(CMAKE_EXE_LINKER_FLAGS "-fuse-ld=lld")
else()
    message(STATUS "Looking for lld - not found")
endif()

set(LOCAL_PIVOT_DIRECTORY "" CACHE STRING "Path to a local copy of the Pivot game engine sources")
if (LOCAL_PIVOT_DIRECTORY STREQUAL "")
    FetchContent_Declare(
        pivot
        GIT_REPOSITORY https://github.com/EIP-Pivot/pivot.git
        GIT_TAG c5939d77262444ec9f1925e387b3c0e4ab58c992
    )
    if (NOT pivot_POPULATED)
        message(STATUS "Populating pivot")
        FetchContent_Populate(pivot)
        add_subdirectory(${pivot_SOURCE_DIR} ${pivot_BINARY_DIR})
        target_compile_definitions(pivot-graphics PUBLIC
            MAX_PROJECTION_LIMIT=1000.0f
        )
    endif()
else()
    add_subdirectory(${LOCAL_PIVOT_DIRECTORY} ${CMAKE_BINARY_DIR}/game-engine)
    target_compile_definitions(pivot-graphics PUBLIC
        MAX_PROJECTION_LIMIT=1000.0f
    )
endif()

add_executable(${PROJECT_NAME} source/main.cxx)

target_include_directories(${PROJECT_NAME} PRIVATE include/)

target_link_libraries(${PROJECT_NAME} PRIVATE pivot-ecs pivot-graphics)
