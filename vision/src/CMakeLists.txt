cmake_minimum_required(VERSION 3.0.0)

find_package( CURL REQUIRED )
if(CURL_FOUND)
   message("Found CURL")
   message("CURL_LIBRARIES: " ${CURL_LIBRARIES})
endif(CURL_FOUND)
include_directories(${CURL_INCLUDE_DIRS})

link_directories( /usr/lib/x86_64-linux-gnu )


find_package( OpenCV REQUIRED )
if(OpenCV_FOUND)
   message("Found OpenCV")
   message("Includes: " ${OpenCV_INCLUDE_DIRS})
   message("OpenCV_LIBS: " ${OpenCV_LIBS})
endif(OpenCV_FOUND)


include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories( actuators )
include_directories( planning )
include_directories( sensors )
include_directories( utilities )

add_subdirectory( actuators )
add_subdirectory( planning )
add_subdirectory( presentation )
add_subdirectory( sensors )
add_subdirectory( utilities )

#file(GLOB_RECURSE source_list "*.cpp" "*.hpp")
#add_executable(RSControl ${source_list})

#add_definitions( -DCURL_STATICLIB )

add_executable(RSControl main.cpp )


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lcurl")

include(CPack)

# TODO LINK libopencv_videoio

#target_link_libraries( RSControl ${OpenCV_LIBS} ${CURL_LIBRARIES} curl actuators planning presentation sensors utilities )
target_link_libraries( RSControl ${OpenCV_LIBS}  opencv_core opencv_highgui actuators planning presentation sensors utilities )




