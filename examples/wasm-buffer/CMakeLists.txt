add_library(wasm-buffer STATIC "${CMAKE_CURRENT_LIST_DIR}/main.cpp")

target_link_libraries(wasm-buffer PUBLIC cui)
set_target_properties(wasm-buffer PROPERTIES FOLDER "examples")
