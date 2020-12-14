find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

# Locate libcURL
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})

set( actuators_source_files
    HTTPClient.cpp
    HTTPClient.h
)
add_library(actuators ${actuators_source_files})
