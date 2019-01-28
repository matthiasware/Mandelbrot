cmake_minimum_required(VERSION 3.11)
project(qt-tutorial LANGUAGES CXX)

set(CMAKE_BUILD_TYPE "Release")

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(main main.cc mandelbrot_widget.cc mandelbrot.h)
target_compile_features(main PRIVATE cxx_std_14)
target_link_libraries(main PRIVATE Qt5::Widgets)
set_target_properties(main PROPERTIES AUTOMOC TRUE)
target_compile_options(main PRIVATE -march=native)



find_package(OpenMP REQUIRED)

if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

find_package(benchmark REQUIRED)
find_package(Threads REQUIRED)

add_executable(bench mandelbench.cc mandelbrot.h)
target_link_libraries(bench PRIVATE Threads::Threads benchmark)
target_link_libraries(bench PRIVATE ${OpenMP_CXX_FLAGS})
target_compile_options(bench PRIVATE -march=native)


# https://github.com/magic-sph/magic/blob/master/cmake/FindSSE.cmake
add_executable(google google.cc mandelbrot.h)
target_link_libraries(google PRIVATE ${OpenMP_CXX_FLAGS})
target_compile_options(google PRIVATE -march=native)
