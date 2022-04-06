cmake_minimum_required(VERSION 3.15)
project(ClipToFile CXX)

add_executable(ClipToFile ${CMAKE_CURRENT_LIST_DIR}/ClipToFile.cpp ${CMAKE_CURRENT_LIST_DIR}/ClipToFile.manifest)
set_target_properties(ClipToFile PROPERTIES 
	CXX_STANDARD 11 
	CXX_STANDARD_REQUIRED ON
	MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
target_link_libraries(ClipToFile kernel32 user32 gdi32)