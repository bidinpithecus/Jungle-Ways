cmake_minimum_required(VERSION 3.12)
project(Game)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -finline-functions -funroll-loops -flto")

# Add source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Add executable
add_executable(main ${SOURCES})

# Link libraries
target_link_libraries(main PRIVATE GL SDL2 SDL2_ttf m)
