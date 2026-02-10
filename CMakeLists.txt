cmake_minimum_required(VERSION 3.21)
project(Project_Cpp_Group30)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)
file(GLOB ALL_SOURCES "sources/*.cpp" "main.cpp")
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/asset" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
add_executable(Project_Cpp_Group30 sources/main.cpp include/constants.h)


target_link_libraries(Project_Cpp_Group30 mingw32 SDL2main SDL2)
target_link_libraries(Project_Cpp_Group30 SDL2_image SDL2_ttf)