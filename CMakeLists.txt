cmake_minimum_required(VERSION 3.0.0)
project(testingcpp VERSION 0.1.0)

include(CTest)
enable_testing()

set(_opencv_version 4)
find_package(OpenCV 4 REQUIRED COMPONENTS imgproc highgui ximgproc REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_library(hungarian hungarian.cpp)
target_link_libraries(
    hungarian
    opencv_highgui
    opencv_ximgproc
    opencv_imgproc
)

add_executable(testingcpp main.cpp)
target_link_libraries(
    testingcpp
    hungarian
    opencv_highgui
    opencv_ximgproc
    opencv_imgproc
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
