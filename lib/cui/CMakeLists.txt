include(GroupSources)

file(
  GLOB_RECURSE SOURCES
  LIST_DIRECTORIES false
  CONFIGURE_DEPENDS
  ${PROJECT_SOURCE_DIR}/include/cui/${MODULE}/*.h
  ${PROJECT_SOURCE_DIR}/include/cui/${MODULE}/*.hpp
  ${PROJECT_SOURCE_DIR}/include/cui/${MODULE}/*.inl
  ${PROJECT_SOURCE_DIR}/include/cui/${MODULE}/*.cpp
  ${PROJECT_SOURCE_DIR}/lib/cui/${MODULE}/*.h
  ${PROJECT_SOURCE_DIR}/lib/cui/${MODULE}/*.hpp
  ${PROJECT_SOURCE_DIR}/lib/cui/${MODULE}/*.inl
  ${PROJECT_SOURCE_DIR}/lib/cui/${MODULE}/*.cmake
  ${PROJECT_SOURCE_DIR}/lib/cui/${MODULE}/*.cpp
  ${PROJECT_SOURCE_DIR}/lib/cui/${MODULE}/*.md)

add_library(cui STATIC ${SOURCES})

target_include_directories(
  cui
  PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
         $<INSTALL_INTERFACE:include>
  PRIVATE ${PROJECT_SOURCE_DIR}/lib)

target_link_libraries(cui PUBLIC adafruit fmt m3)

group_sources("${CMAKE_CURRENT_LIST_DIR}" "${PROJECT_SOURCE_DIR}/include/cui")

set_target_properties(cui PROPERTIES FOLDER "lib")

option(${PROJECT_NAME}_WITH_PEDANTIC_ASSERT "Enable extended asserts" OFF)
if(${PROJECT_NAME}_WITH_PEDANTIC_ASSERT)
  target_compile_definitions(cui PUBLIC CUI_HAS_PEDANTIC_ASSERT)
endif()

if(${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.16)
  option(${PROJECT_NAME}_WITH_PCH "Enable the precompiled header compilation"
         ON)
  if(${PROJECT_NAME}_WITH_PCH)
    target_precompile_headers(cui PRIVATE ${CMAKE_CURRENT_LIST_DIR}/pch.hpp)
  endif()
endif()
