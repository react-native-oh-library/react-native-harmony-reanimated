cmake_minimum_required(VERSION 3.4.1)
set(CMAKE_VERBOSE_MAKEFILE on)

add_compile_definitions(REANIMATED_VERSION=3.18.0)
add_compile_definitions(JS_RUNTIME_HERMES=1)
add_compile_definitions(RCT_NEW_ARCH_ENABLED=1)
add_compile_definitions(HERMES_ENABLE_DEBUGGER=1)

set(REANIMATED_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Common")
set(PATCHES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/patches")

file(GLOB rnoh_reanimated_SRC CONFIGURE_DEPENDS *.cpp)

add_library(rnoh_reanimated SHARED ${rnoh_reanimated_SRC})

target_include_directories(
  rnoh_reanimated
  PRIVATE
  "${REANIMATED_SRC_DIR}/cpp/reanimated/AnimatedSensor"
  "${REANIMATED_SRC_DIR}/cpp/reanimated/Fabric"
  "${REANIMATED_SRC_DIR}/cpp/reanimated/hidden_headers"
  "${REANIMATED_SRC_DIR}/cpp/reanimated/LayoutAnimations"
  "${REANIMATED_SRC_DIR}/cpp/reanimated/NativeModules"
  "${REANIMATED_SRC_DIR}/cpp/reanimated/ReanimatedRuntime"
  "${REANIMATED_SRC_DIR}/cpp/Worklets/Registries"
  "${REANIMATED_SRC_DIR}/cpp/Worklets/SharedItems"
  "${REANIMATED_SRC_DIR}/cpp/reanimated/Tools"
  "${REANIMATED_SRC_DIR}/cpp/worklets/Tools"
  "${REANIMATED_SRC_DIR}/cpp"
  "${REANIMATED_SRC_DIR}/cpp/worklets/WorkletRuntime"
  "${REANIMATED_SRC_DIR}/cpp/reanimated/RuntimeDecorators"
)
target_sources(rnoh_reanimated 
  PRIVATE 
  # BEGIN: patches
  "${PATCHES_DIR}/Fabric/ShadowTreeCloner.cpp"
  "${PATCHES_DIR}/NativeModules/ReanimatedModuleProxy.cpp"
  "${PATCHES_DIR}/AnimatedSensor/AnimatedSensorModule.cpp"
  # END: patches
  "${REANIMATED_SRC_DIR}/cpp/reanimated/Fabric/PropsRegistry.cpp"
  "${REANIMATED_SRC_DIR}/cpp/reanimated/Fabric/ReanimatedCommitHook.cpp"
  "${REANIMATED_SRC_DIR}/cpp/reanimated/Fabric/ReanimatedMountHook.cpp"
  "${REANIMATED_SRC_DIR}/cpp/reanimated/LayoutAnimations/LayoutAnimationsManager.cpp"
  "${REANIMATED_SRC_DIR}/cpp/reanimated/LayoutAnimations/LayoutAnimationsProxy.cpp"
  "${REANIMATED_SRC_DIR}/cpp/reanimated/LayoutAnimations/LayoutAnimationsUtils.cpp"
  "${REANIMATED_SRC_DIR}/cpp/reanimated/NativeModules/ReanimatedModuleProxySpec.cpp"
  "${REANIMATED_SRC_DIR}/cpp/reanimated/RuntimeDecorators/RNRuntimeDecorator.cpp"
  "${REANIMATED_SRC_DIR}/cpp/worklets/WorkletRuntime/ReanimatedHermesRuntime.cpp"
  "${REANIMATED_SRC_DIR}/cpp/worklets/WorkletRuntime/ReanimatedRuntime.cpp"
  "${REANIMATED_SRC_DIR}/cpp/worklets/WorkletRuntime/WorkletRuntime.cpp"
  "${REANIMATED_SRC_DIR}/cpp/Worklets/Registries/WorkletRuntimeRegistry.cpp"
  "${REANIMATED_SRC_DIR}/cpp/Worklets/Registries/EventHandlerRegistry.cpp"
  "${REANIMATED_SRC_DIR}/cpp/Worklets/SharedItems/Shareables.cpp"
  "${REANIMATED_SRC_DIR}/cpp/Worklets/Tools/AsyncQueue.cpp"
  "${REANIMATED_SRC_DIR}/cpp/reanimated/Tools/FeaturesConfig.cpp"
  "${REANIMATED_SRC_DIR}/cpp/Worklets/Tools/JSISerializer.cpp"
  "${REANIMATED_SRC_DIR}/cpp/Worklets/Tools/JSLogger.cpp"
  "${REANIMATED_SRC_DIR}/cpp/Worklets/Tools/JSScheduler.cpp"
  "${REANIMATED_SRC_DIR}/cpp/Worklets/Tools/ReanimatedJSIUtils.cpp"
  "${REANIMATED_SRC_DIR}/cpp/Worklets/Tools/ReanimatedVersion.cpp"
  "${REANIMATED_SRC_DIR}/cpp/reanimated/RuntimeDecorators/UIRuntimeDecorator.cpp"
  "${REANIMATED_SRC_DIR}/cpp/Worklets/Tools/UIScheduler.cpp"
  "${REANIMATED_SRC_DIR}/cpp/Worklets/Tools/WorkletEventHandler.cpp"
  "${REANIMATED_SRC_DIR}/cpp/worklets/NativeModules/WorkletsModuleProxy.cpp"
  "${REANIMATED_SRC_DIR}/cpp/worklets/NativeModules/WorkletsModuleProxySpec.cpp"
  "${REANIMATED_SRC_DIR}/cpp/worklets/WorkletRuntime/RNRuntimeWorkletDecorator.cpp"
  "${REANIMATED_SRC_DIR}/cpp/worklets/WorkletRuntime/WorkletRuntimeDecorator.cpp"
  )

target_include_directories(rnoh_reanimated PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(rnoh_reanimated PUBLIC rnoh)

