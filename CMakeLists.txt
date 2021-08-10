cmake_minimum_required(VERSION 3.11)
project(user-interface)

include(FetchContent)

find_package(Vulkan REQUIRED)

set(CC clang)
set(CXX clang++)
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

FetchContent_Declare(
    pivot
    GIT_REPOSITORY https://github.com/EIP-Pivot/game-engine.git
    GIT_TAG f8dfb18e74126350a71f5a069b2a9ed70551d9c0
)
if (NOT pivot_POPULATED)
    message(STATUS "Populating pivot")
    FetchContent_Populate(pivot)
    add_subdirectory(${pivot_SOURCE_DIR} ${pivot_BINARY_DIR})
endif()

add_executable(${PROJECT_NAME} source/main.cxx
                               source/Window.cxx
)

target_include_directories(${PROJECT_NAME} PRIVATE include/)

target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)
target_link_libraries(${PROJECT_NAME} PRIVATE pivot-ecs pivot-graphics)
