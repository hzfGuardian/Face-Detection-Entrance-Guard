project( FaceTracker )
find_package( OpenCV REQUIRED )
add_executable( FaceTracker FaceTracker )
target_link_libraries( FaceTracker ${OpenCV_LIBS} )
