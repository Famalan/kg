cmake_minimum_required(VERSION 3.10)
project(lab)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Добавляем поддержку OpenMP
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(SFML 2.6 COMPONENTS system window graphics REQUIRED)

add_executable(lab6 lab6_v2.cpp)

target_link_libraries(lab6
    PRIVATE
    OpenGL::GL
    GLEW::GLEW
    glfw
    sfml-system
    sfml-window
    sfml-graphics
)

if(OpenMP_CXX_FOUND)
    target_link_libraries(lab6 PRIVATE OpenMP::OpenMP_CXX)
endif()

if(APPLE)
    target_link_libraries(lab6 PRIVATE "-framework OpenGL")
endif() 