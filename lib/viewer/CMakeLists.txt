include(GroupSources)

file(
  GLOB_RECURSE SOURCES
  LIST_DIRECTORIES false
  CONFIGURE_DEPENDS
  ${PROJECT_SOURCE_DIR}/include/viewer/${MODULE}/*.hpp
  ${PROJECT_SOURCE_DIR}/include/viewer/${MODULE}/*.inl
  ${PROJECT_SOURCE_DIR}/include/viewer/${MODULE}/*.cpp
  ${PROJECT_SOURCE_DIR}/lib/viewer/${MODULE}/*.hpp
  ${PROJECT_SOURCE_DIR}/lib/viewer/${MODULE}/*.inl
  ${PROJECT_SOURCE_DIR}/lib/viewer/${MODULE}/*.cmake
  ${PROJECT_SOURCE_DIR}/lib/viewer/${MODULE}/*.cpp
  ${PROJECT_SOURCE_DIR}/lib/viewer/${MODULE}/*.md)

add_library(viewer STATIC ${SOURCES})

target_link_libraries(
  viewer
  PUBLIC cui
  PRIVATE fmt glfw glbinding imgui nativefiledialog
          $<$<PLATFORM_ID:Windows>:Shcore>)

target_include_directories(
  viewer
  PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
         $<INSTALL_INTERFACE:include>
  PRIVATE ${PROJECT_SOURCE_DIR}/lib)

target_compile_definitions(viewer PRIVATE GLFW_INCLUDE_NONE
                                          $<$<PLATFORM_ID:Windows>:NOMINMAX>)

group_sources("${CMAKE_CURRENT_LIST_DIR}"
              "${PROJECT_SOURCE_DIR}/include/viewer")

set_target_properties(viewer PROPERTIES FOLDER "lib")

if(${PROJECT_NAME}_WITH_PCH)
  target_precompile_headers(viewer PRIVATE ${CMAKE_CURRENT_LIST_DIR}/pch.hpp)
endif()
