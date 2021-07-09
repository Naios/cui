add_library(wasm-minimal STATIC "${CMAKE_CURRENT_LIST_DIR}/main.cpp")

target_link_libraries(wasm-minimal PUBLIC cui)
set_target_properties(wasm-minimal PROPERTIES FOLDER "examples")
