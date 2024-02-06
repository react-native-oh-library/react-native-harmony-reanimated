cmake_minimum_required(VERSION 3.13)
set(CMAKE_VERBOSE_MAKEFILE on)

file(GLOB rnoh_gesture_handler_SRC CONFIGURE_DEPENDS *.cpp)
add_library(rnoh_gesture_handler SHARED ${rnoh_gesture_handler_SRC})
target_include_directories(rnoh_gesture_handler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(rnoh_gesture_handler PUBLIC rnoh)

