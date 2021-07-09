add_library(wasm-bitmap2 STATIC "${CMAKE_CURRENT_LIST_DIR}/main.cpp")

target_link_libraries(wasm-bitmap2 PUBLIC cui)
set_target_properties(wasm-bitmap2 PROPERTIES FOLDER "examples")
