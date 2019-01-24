cmake_minimum_required(VERSION 3.11)
project(qt-tutorial LANGUAGES CXX)

set(CMAKE_BUILD_TYPE "Release")

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(main main.cc mandelbrot_widget.cc mandelbrot.h)
target_compile_features(main PRIVATE cxx_std_14)
target_link_libraries(main PRIVATE Qt5::Widgets)
set_target_properties(main PROPERTIES AUTOMOC TRUE)

add_executable(bench mandelbench.cc mandelbrot.h)


find_package(benchmark REQUIRED)
find_package(Threads REQUIRED)
add_executable(google google.cc)
target_link_libraries(google Threads::Threads benchmark)