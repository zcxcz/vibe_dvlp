cmake_minimum_required(VERSION 3.10)
project(vibe)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Check if json.hpp exists
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/json.hpp")
    message(FATAL_ERROR "json.hpp not found in ${CMAKE_CURRENT_SOURCE_DIR}/src")
endif()

# Add executable
add_executable(vibe src/vibe.cpp src/crop.cpp)

# Include header directories
target_include_directories(vibe PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hls_lib
)

# Add compilation options
target_compile_options(vibe PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# Link system libraries
target_link_libraries(vibe PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:stdc++fs>
)

# Installation rules
install(TARGETS vibe DESTINATION bin)
install(FILES data/vibe.json DESTINATION share/vibe)
