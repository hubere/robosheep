cmake_minimum_required(VERSION 3.0.0)
project(RSControl VERSION 0.1.0)

include(CTest)
enable_testing()

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_subdirectory( src )


#file(GLOB_RECURSE source_list "*.cpp" "*.hpp")

# add_executable(RSControl ${source_list})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)




