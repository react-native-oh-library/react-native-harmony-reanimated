project(rnapp)
cmake_minimum_required(VERSION 3.4.1)
set(CMAKE_SKIP_BUILD_RPATH TRUE)
set(OH_MODULES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../oh_modules")
set(RNOH_APP_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(NODE_MODULES "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../node_modules")
set(RNOH_CPP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../node_modules/react-native-harmony/harmony/cpp")
set(CMAKE_CXX_FLAGS "-fstack-protector-strong -Wl,-z,relro,-z,now,-z,noexecstack -s -fPIE -pie")

file(GLOB GENERATED_CPP_FILES "./generated/*.cpp")

add_subdirectory("${RNOH_CPP_DIR}" ./rn)
add_subdirectory("${OH_MODULES_DIR}/rnoh-gesture-handler/src/main/cpp" ./gesture_handler)
add_subdirectory("${OH_MODULES_DIR}/rnoh-reanimated/src/main/cpp" ./reanimated)


add_library(rnoh_app SHARED
    ${GENERATED_CPP_FILES}
    "./PackageProvider.cpp"
    "${RNOH_CPP_DIR}/RNOHAppNapiBridge.cpp"
)
target_link_libraries(rnoh_app PUBLIC rnoh)

target_link_libraries(rnoh_app PUBLIC rnoh_reanimated)
target_link_libraries(rnoh_app PUBLIC rnoh_gesture_handler)

