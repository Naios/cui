cmake_minimum_required(VERSION 3.13)
project(cui)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)
include(cmake/CMakeLists.txt)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_subdirectory(dep)
add_subdirectory(lib)

if(BUILD_TESTING)
  message(STATUS "Enable tests")

  enable_testing()
  add_subdirectory(test)
endif()

option(${PROJECT_NAME}_WITH_EXAMPLES "Enable examples" ON)
if(${PROJECT_NAME}_WITH_EXAMPLES)
  message(STATUS "Enable examples")

  add_subdirectory(examples)
endif()

add_subdirectory(tools)
