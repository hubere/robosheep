find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

set( presentation_source_files
    GUI.cpp
    GUI.h
)
add_library(presentation ${presentation_source_files})


