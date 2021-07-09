add_library(wasm-basic STATIC "${CMAKE_CURRENT_LIST_DIR}/main.cpp")

target_link_libraries(wasm-basic PUBLIC cui)
set_target_properties(wasm-basic PROPERTIES FOLDER "examples")
