cmake_minimum_required(VERSION 3.13)
set(CMAKE_VERBOSE_MAKEFILE on)

file(GLOB rnoh_safe_area_SRC CONFIGURE_DEPENDS *.cpp)
add_library(rnoh_safe_area SHARED ${rnoh_safe_area_SRC})
target_include_directories(rnoh_safe_area PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(rnoh_safe_area PUBLIC rnoh)