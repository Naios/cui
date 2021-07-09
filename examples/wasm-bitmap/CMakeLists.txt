add_library(wasm-bitmap STATIC "${CMAKE_CURRENT_LIST_DIR}/main.cpp")

target_link_libraries(wasm-bitmap PUBLIC cui)
set_target_properties(wasm-bitmap PROPERTIES FOLDER "examples")
