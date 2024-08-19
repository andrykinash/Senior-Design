cmake_minimum_required(VERSION 3.1...3.23)

project(Senior_Design)

find_package(CGAL)

find_package(CGAL REQUIRED OPTIONAL_COMPONENTS Qt5)

set(CMAKE_SUPPRESS_REGENERATION true)

add_executable(ArtGallery ArtGallery.cpp MainWindow.cpp MainWindow.h Triangulation.cpp Triangulation.h ez.h ModifiedGraphicsView.h ModifiedGraphicsView.cpp)

target_link_libraries(ArtGallery PUBLIC CGAL::CGAL)

target_compile_options(ArtGallery PUBLIC /Zc:preprocessor)

if(CGAL_Qt5_FOUND)
  target_link_libraries(ArtGallery PUBLIC CGAL::CGAL_Basic_viewer)
endif()
