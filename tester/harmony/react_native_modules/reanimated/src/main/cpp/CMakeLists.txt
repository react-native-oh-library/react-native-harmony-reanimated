cmake_minimum_required(VERSION 3.13)
set(CMAKE_VERBOSE_MAKEFILE on)

add_compile_definitions(REANIMATED_VERSION=3.6.0)
add_compile_definitions(REACT_NATIVE_MINOR_VERSION=72)
add_compile_definitions(JS_RUNTIME_HERMES=1)
add_compile_definitions(RCT_NEW_ARCH_ENABLED=1)
add_compile_definitions(HERMES_ENABLE_DEBUGGER=1)



set(REANIMATED_SRC_DIR "${NODE_MODULES}/react-native-reanimated/Common")
set(PATCHES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/patches")

file(GLOB rnoh_reanimated_SRC CONFIGURE_DEPENDS *.cpp)

add_library(rnoh_reanimated SHARED ${rnoh_reanimated_SRC})

target_include_directories(
  rnoh_reanimated
  PRIVATE
  # BEGIN: patches
  "${CMAKE_CURRENT_SOURCE_DIR}/patches/Tools/"
  # END: patches
  "${REANIMATED_SRC_DIR}/cpp/AnimatedSensor"
  "${REANIMATED_SRC_DIR}/cpp/Fabric"
  "${REANIMATED_SRC_DIR}/cpp/hidden_headers"
  "${REANIMATED_SRC_DIR}/cpp/LayoutAnimations"
  "${REANIMATED_SRC_DIR}/cpp/NativeModules"
  "${REANIMATED_SRC_DIR}/cpp/ReanimatedRuntime"
  "${REANIMATED_SRC_DIR}/cpp/Registries"
  "${REANIMATED_SRC_DIR}/cpp/SharedItems"
  "${REANIMATED_SRC_DIR}/cpp/Tools"
)
target_sources(rnoh_reanimated 
  PRIVATE 
  # BEGIN: patches
  "${PATCHES_DIR}/ReanimatedRuntime/WorkletRuntimeDecorator.cpp"
  # END: patches
  "${REANIMATED_SRC_DIR}/cpp/AnimatedSensor/AnimatedSensorModule.cpp"
  "${REANIMATED_SRC_DIR}/cpp/Fabric/PropsRegistry.cpp"
  "${REANIMATED_SRC_DIR}/cpp/Fabric/ReanimatedCommitHook.cpp"
  "${REANIMATED_SRC_DIR}/cpp/Fabric/ReanimatedCommitMarker.cpp"
  "${REANIMATED_SRC_DIR}/cpp/Fabric/ReanimatedMountHook.cpp"
  "${REANIMATED_SRC_DIR}/cpp/Fabric/ShadowTreeCloner.cpp"
  "${REANIMATED_SRC_DIR}/cpp/LayoutAnimations/LayoutAnimationsManager.cpp"
  "${REANIMATED_SRC_DIR}/cpp/NativeModules/NativeReanimatedModule.cpp"
  "${REANIMATED_SRC_DIR}/cpp/NativeModules/NativeReanimatedModuleSpec.cpp"
  "${REANIMATED_SRC_DIR}/cpp/ReanimatedRuntime/RNRuntimeDecorator.cpp"
  "${REANIMATED_SRC_DIR}/cpp/ReanimatedRuntime/ReanimatedHermesRuntime.cpp"
  "${REANIMATED_SRC_DIR}/cpp/ReanimatedRuntime/ReanimatedRuntime.cpp"
  "${REANIMATED_SRC_DIR}/cpp/ReanimatedRuntime/WorkletRuntime.cpp"
  "${REANIMATED_SRC_DIR}/cpp/ReanimatedRuntime/WorkletRuntimeRegistry.cpp"
  "${REANIMATED_SRC_DIR}/cpp/Registries/EventHandlerRegistry.cpp"
  "${REANIMATED_SRC_DIR}/cpp/SharedItems/Shareables.cpp"
  "${REANIMATED_SRC_DIR}/cpp/Tools/AsyncQueue.cpp"
  "${REANIMATED_SRC_DIR}/cpp/Tools/FeaturesConfig.cpp"
  "${REANIMATED_SRC_DIR}/cpp/Tools/JSISerializer.cpp"
  "${REANIMATED_SRC_DIR}/cpp/Tools/JSLogger.cpp"
  "${REANIMATED_SRC_DIR}/cpp/Tools/JSScheduler.cpp"
  "${REANIMATED_SRC_DIR}/cpp/Tools/ReanimatedJSIUtils.cpp"
  "${REANIMATED_SRC_DIR}/cpp/Tools/ReanimatedVersion.cpp"
  "${REANIMATED_SRC_DIR}/cpp/Tools/UIRuntimeDecorator.cpp"
  "${REANIMATED_SRC_DIR}/cpp/Tools/UIScheduler.cpp"
  "${REANIMATED_SRC_DIR}/cpp/Tools/WorkletEventHandler.cpp"
  )

target_include_directories(rnoh_reanimated PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(rnoh_reanimated PUBLIC rnoh)

