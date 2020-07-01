cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(Raytracer)

find_package(LibXml2 REQUIRED)

add_executable(raytracer
    src/avsio.c
    src/list.c
    src/objects.c
    src/raytrace.c
    src/raytracer.c
    src/vector.c
    src/xmlio.c)
target_include_directories(raytracer PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(raytracer m LibXml2::LibXml2)