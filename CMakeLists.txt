cmake_minimum_required(VERSION 3.11)
project(qt-tutorial LANGUAGES CXX)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(main main.cc mandelbrot_widget.cc)
target_compile_features(main PRIVATE cxx_std_14)
target_link_libraries(main PRIVATE Qt5::Widgets)
set_target_properties(main PROPERTIES AUTOMOC TRUE)