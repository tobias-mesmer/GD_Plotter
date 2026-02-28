cmake_minimum_required(VERSION 3.20)
project(GD_Plotter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Automatically fetch ImGui
include(FetchContent)
FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG v1.92.6
)
FetchContent_MakeAvailable(imgui)

# Change bin output destination
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/$<CONFIG>) #   output destination for .exe
#set_target_properties(${PROJECT_NAME}
#        PROPERTIES
#        LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/$<CONFIG> #      output destination for library on unix
#        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/$<CONFIG> #      output destination for .dll on Windows
#)

add_executable(${PROJECT_NAME} src/main.cpp)