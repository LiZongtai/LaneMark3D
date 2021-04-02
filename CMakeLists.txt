cmake_minimum_required(VERSION 3.17)
project(LaneMark3D)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)
link_directories(${PROJECT_SOURCE_DIR}/lib)

set(CMAKE_CXX_STANDARD 14)

add_executable(LaneMark3D main.cpp
        src/glad.c
        stb_image.cpp
        LaneDetection/Lane.cpp
        LaneDetection/Lane.h
        LaneDetection/LaneDetectionModule.cpp
        LaneDetection/LaneDetectionModule.h
        LaneDetection/LaneMark.cpp
        LaneDetection/LaneMark.h
        Shader.cpp
        Shader.h)

set(OpenCV_DIR H:/C/opencv450/build/x64/vc15/lib)
find_package(OpenCV REQUIRED)
#如果cmake找不到库就把上面一条换为下面的语句
include_directories(H:/C/opencv450/build/include/opencv2)

target_link_libraries(LaneMark3D ${PROJECT_SOURCE_DIR}/lib/glfw3.lib ${OpenCV_LIBS})