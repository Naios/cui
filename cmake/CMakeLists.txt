# Select the compiler specific cmake file
set(MSVC_ID "MSVC")
if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
  include(${CMAKE_CURRENT_LIST_DIR}/compiler/clang.cmake)
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
  include(${CMAKE_CURRENT_LIST_DIR}/compiler/gcc.cmake)
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL ${MSVC_ID})
  include(${CMAKE_CURRENT_LIST_DIR}/compiler/msvc.cmake)
else()
  message(FATAL_ERROR "Unknown compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()

# Add the modules to the path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/modules")
