cmake_minimum_required(VERSION 3.0.0)
project(RSControl VERSION 0.1.0)

include(CTest)
enable_testing()

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(RSControl src/main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)


target_link_libraries( RSControl ${OpenCV_LIBS} )




