find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

set( planning_source_files
    Garden.cpp
    Garden.h
    Planer.cpp
    Planer.h
)
add_library(planning ${planning_source_files})
