cmake_minimum_required(VERSION 3.10)
project(mandelbrot)

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(
	${PROJECT_NAME}
	INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
)
target_compile_definitions(
	${PROJECT_NAME}
	INTERFACE
	MANDELBROT_AVX_
	MANDELBROT_OMP_
)