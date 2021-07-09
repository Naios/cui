add_library(m3-check STATIC "${CMAKE_CURRENT_LIST_DIR}/main.cpp")

set_target_properties(m3-check PROPERTIES FOLDER "test")
