cmake_minimum_required(VERSION 3.20)
project(SolumEngine VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ----- Find required libraries -----
find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)

# ----- GLAD (embedded in src/glad) -----
add_library(glad STATIC src/glad/glad.c)
target_include_directories(glad PUBLIC src/glad)

# ----- ImGui + ImGui-SFML -----
set(IMGUI_DIR src/imgui)
add_library(imgui STATIC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui-SFML.cpp
)
target_include_directories(imgui PUBLIC ${IMGUI_DIR})
target_link_libraries(imgui PUBLIC SFML::Graphics OpenGL::GL)

# ----- Engine source files -----
file(GLOB_RECURSE ENGINE_SOURCES
    src/core/*.cpp
    src/renderer/*.cpp
    src/platform/*.cpp
    src/systems/*.cpp
    src/entities/*.cpp
)

# ----- Main executable -----
add_executable(SolumEngine ${ENGINE_SOURCES})
target_include_directories(SolumEngine PUBLIC src)
target_link_libraries(SolumEngine PUBLIC
    SFML::Graphics
    SFML::Window
    SFML::System
    OpenGL::GL
    glm::glm
    glad
    imgui
)