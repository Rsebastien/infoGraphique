cmake_minimum_required(VERSION 3.21)
project(playGround)

set(CMAKE_CXX_STANDARD 17)

# find the required packages
find_package(OpenGL REQUIRED)
find_package(X11 REQUIRED)

set(LIBS glfw GLU X11 Xrandr Xinerama Xi Xxf86vm Xcursor GL pthread glut)

FILE(GLOB_RECURSE
        folder_source
        ${CMAKE_SOURCE_DIR}/src/*.cpp
        ${CMAKE_SOURCE_DIR}/src/*.c
        ${CMAKE_SOURCE_DIR}/includes/*.cpp
        ${CMAKE_SOURCE_DIR}/includes/*.c
        )

FILE(GLOB_RECURSE
        header_source
        ${CMAKE_SOURCE_DIR}/src/*.hpp
        ${CMAKE_SOURCE_DIR}/src/*.h
        ${CMAKE_SOURCE_DIR}/includes/*.h
        )

include_directories(${CMAKE_SOURCE_DIR}/includes)

add_executable(main ${folder_source} ${header_source})

target_link_libraries(main ${LIBS} ${CMAKE_DL_LIBS})