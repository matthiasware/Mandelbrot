cmake_minimum_required(VERSION 3.4)
project(imgwrite)

add_library(
	${PROJECT_NAME}
	imgwrite.cc)

target_include_directories(
	${PROJECT_NAME}
	INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..)
