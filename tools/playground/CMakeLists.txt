add_executable(playground main.cpp)
target_link_libraries(playground PUBLIC cui viewer)

set_target_properties(playground PROPERTIES FOLDER "tools")

target_include_directories(
  playground
  PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
         $<INSTALL_INTERFACE:include>)
