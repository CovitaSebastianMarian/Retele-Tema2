cmake_minimum_required(VERSION 3.10)
project(app)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

# Sursele ImGui
file(GLOB IMGUI_SOURCES
    imgui/*.cpp
    imgui/*.h
    imgui/backends/*.cpp
    imgui/backends/*.h
)

# Sursele proiectului
set(SOURCES
    main.cpp
    ${IMGUI_SOURCES}
)

add_executable(app ${SOURCES})

# Include directoarele ImGui
target_include_directories(app PRIVATE imgui imgui/backends)

# Link librării
target_link_libraries(app PRIVATE OpenGL::GL glfw GLEW)
