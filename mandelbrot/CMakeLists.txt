cmake_minimum_required(VERSION 3.10)
project(mandelbrot)

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(
	${PROJECT_NAME}
	INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
)

find_package(OpenMP)
IF(OpenMP_FOUND)
	target_link_libraries(
		${PROJECT_NAME}
		INTERFACE
		OpenMP::OpenMP_CXX)
ENDIF()

# for AVX2
target_compile_options(
	${PROJECT_NAME}
	INTERFACE
	-march=native
)

find_package(benchmark)
find_package(Threads)

IF(benchmark_FOUND AND Threads_FOUND)
	add_executable(bench mandelbench.cc)
	target_link_libraries(
		bench
		PRIVATE
		Threads::Threads
		benchmark
		${PROJECT_NAME})
	target_compile_options(
		bench
		PRIVATE
		-O2)
ENDIF()