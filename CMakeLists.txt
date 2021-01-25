cmake_minimum_required (VERSION 2.6)
project (onishiki)
set (ponkotu_VERSION_MAJOR 0)
set (ponkotu_VERSION_MINOR 1)
set (EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

configure_file (
	"${PROJECT_SOURCE_DIR}/src/include/config.h.in"
	"${PROJECT_SOURCE_DIR}/src/include/config.h"
	)

include_directories("${PROJECT_SOURCE_DIR}/src/include/")
add_executable(oni src/main.c)
