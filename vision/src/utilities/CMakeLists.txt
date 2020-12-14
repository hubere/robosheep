find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

set( utilities_source_files
    OpenCVUtils.cpp
    OpenCVUtils.h
    Stopwatch.cpp
    Stopwatch.h
    VirtualSheep.cpp
    VirtualSheep.h
)
add_library(utilities ${utilities_source_files})
