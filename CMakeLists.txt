cmake_minimum_required(VERSION 3.10)
project(cohen_sutherland)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

add_executable(cohen_sutherland ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})