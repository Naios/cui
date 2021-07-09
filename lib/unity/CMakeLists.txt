add_library(unity STATIC "${CMAKE_CURRENT_LIST_DIR}/unity.cpp")
target_include_directories(
  unity
  PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
         $<INSTALL_INTERFACE:include>
  PRIVATE ${PROJECT_SOURCE_DIR}/lib)
set_target_properties(unity PROPERTIES FOLDER "lib")
