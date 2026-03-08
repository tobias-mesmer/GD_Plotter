cmake_minimum_required(VERSION 3.20)
project(GD_Plotter LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# ImGui
FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG v1.92.6
)
FetchContent_MakeAvailable(imgui)

# GLFW
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

# GLAD
add_library(glad external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

find_package(OpenGL REQUIRED)

add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        glfw
        glad
        OpenGL::GL
)

# Change bin output destination
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/$<CONFIG>) #   output destination for .exe
#set_target_properties(${PROJECT_NAME}
#        PROPERTIES
#        LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/$<CONFIG> #      output destination for library on unix
#        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/$<CONFIG> #      output destination for .dll on Windows
#)