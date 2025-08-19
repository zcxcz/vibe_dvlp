cmake_minimum_required(VERSION 3.10)
project(vibe)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置构建输出目录到与compile同级的build目录
set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Check if json.hpp exists
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/json.hpp")
    message(FATAL_ERROR "json.hpp not found in src/")
endif()

# Add executable
add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/alg_info.cpp
    src/alg_crop.cpp
    src/alg_dpc.cpp
    src/hls_crop.cpp
)

# Include header directories
target_include_directories(${PROJECT_NAME} PRIVATE 
    src
    src/hls_lib
)

# Add compilation options
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wno-int-in-bool-context>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# Link system libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:stdc++fs>
)

# Installation rules
install(TARGETS ${PROJECT_NAME} DESTINATION build)
install(FILES data/vibe.json DESTINATION build)