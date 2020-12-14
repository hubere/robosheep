find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

set( sensors_source_files
    ImageAnalyser.cpp
    ImageAnalyser.h
    TrackedColorBlob.cpp
    TrackedColorBlob.h
    TrackedObject.cpp
    TrackedObject.h
    VideoCamera.cpp
    VideoCamera.h
)
add_library(sensors ${sensors_source_files})
