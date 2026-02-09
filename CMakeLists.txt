cmake_minimum_required(VERSION 3.21)
project(Project_Cpp_Group30)

set(CMAKE_CXX_STANDARD 20)

add_executable(Project_Cpp_Group30 Welcometeamate.cpp first_page.h)
target_link_libraries(Project_Cpp_Group30 mingw32 SDL2main SDL2)
target_link_libraries(Project_Cpp_Group30 SDL2_image SDL2_ttf)