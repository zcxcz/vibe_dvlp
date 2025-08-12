cmake_minimum_required(VERSION 3.10)
project(vibe)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置正确的相对路径（相对于compile目录）
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../src)
set(DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../data)

# Check if json.hpp exists
if(NOT EXISTS "${SRC_DIR}/json.hpp")
    message(FATAL_ERROR "json.hpp not found in ${SRC_DIR}")
endif()

# Add executable
add_executable(${PROJECT_NAME} 
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/alg_crop.cpp
    ${SRC_DIR}/hls_crop.cpp
)

# Include header directories
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${SRC_DIR}
    ${SRC_DIR}/hls_lib
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
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(FILES ${DATA_DIR}/vibe.json DESTINATION share/vibe)
