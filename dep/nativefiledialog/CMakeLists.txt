add_library(
  nativefiledialog STATIC
  "${CMAKE_CURRENT_LIST_DIR}/nativefiledialog/src/nfd_common.c"
  "${CMAKE_CURRENT_LIST_DIR}/nativefiledialog/src/nfd_win.cpp")

target_include_directories(
  nativefiledialog
  PUBLIC "${CMAKE_CURRENT_LIST_DIR}/nativefiledialog/src/include"
         "${CMAKE_CURRENT_LIST_DIR}/nativefiledialog/src")

set_target_properties(nativefiledialog PROPERTIES FOLDER "dep")
