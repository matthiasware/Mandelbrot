cmake_minimum_required(VERSION 3.4)
project(mandelviz LANGUAGES CXX)

set(CMAKE_BUILD_TYPE "Release")

add_subdirectory(mandelbrot)
add_subdirectory(imgwrite)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(
	${PROJECT_NAME}
	main.cc
	mandelbrot_widget.cc)

target_link_libraries(
	${PROJECT_NAME}
	PRIVATE
	Qt5::Widgets
	mandelbrot
	imgwrite
)
set_target_properties(
	${PROJECT_NAME}
	PROPERTIES
	AUTOMOC TRUE
)


add_executable(
	mandelbrot2png
	mandelbrot2png.cc
)
target_link_libraries(
	mandelbrot2png
	mandelbrot
	imgwrite
)