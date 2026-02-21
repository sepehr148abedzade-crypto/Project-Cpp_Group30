cmake_minimum_required(VERSION 3.21)
project(Project_Cpp_Group30)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)
file(GLOB ALL_SOURCES "sources/*.cpp" "main.cpp")
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/asset" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
add_executable(Project_Cpp_Group30 sources/main.cpp
        sources/Entity.cpp
        sources/Graphic_Element.cpp
        sources/TextureManager.cpp
        sources/Game.cpp
        sources/constants.cpp
        include/Asset_Loader.h sources/Asset_Loader.cpp
        sources/motion_functions.cpp
        include/motion_functions.h
        sources/operators_functions.cpp
        include/operators_functions.h
        sources/looks_functions.cpp
        include/looks_functions.h
        sources/sound_functions.cpp
        include/sound_functions.h
        sources/sensing_functions.cpp
        include/sensing_functions.h
        sources/Paint_Editor.cpp
        include/Paint_Editor.h
        include/ProjectManager.h
        sources/ProjectManager.cpp
)


target_link_libraries(Project_Cpp_Group30 mingw32 SDL2main SDL2)
target_link_libraries(Project_Cpp_Group30 SDL2_image SDL2_ttf comdlg32 -lSDL2_mixer)