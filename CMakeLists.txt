cmake_minimum_required(VERSION 3.4)
project(qt-tutorial LANGUAGES CXX)

set(CMAKE_BUILD_TYPE "Release")

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(OpenMP REQUIRED)

######################################
# 				MAIN
######################################
add_executable(main main.cc mandelbrot_widget.cc mandelbrot.h)

target_link_libraries(main PRIVATE Qt5::Widgets)
target_link_libraries(main PRIVATE OpenMP::OpenMP_CXX)
set_target_properties(main PROPERTIES AUTOMOC TRUE)
target_compile_options(main PRIVATE -march=native)
target_compile_definitions(main PRIVATE FOO=1)

######################################
# 				MAIN
######################################
find_package(benchmark REQUIRED)
find_package(Threads REQUIRED)

add_executable(bench mandelbench.cc mandelbrot.h)
target_link_libraries(bench PRIVATE Threads::Threads benchmark)
target_link_libraries(bench PUBLIC OpenMP::OpenMP_CXX)
target_compile_options(bench PRIVATE -march=native)
