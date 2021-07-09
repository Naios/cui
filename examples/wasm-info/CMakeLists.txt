add_library(wasm-info STATIC "${CMAKE_CURRENT_LIST_DIR}/main.cpp")

target_link_libraries(wasm-info PUBLIC cui)
set_target_properties(wasm-info PROPERTIES FOLDER "examples")
