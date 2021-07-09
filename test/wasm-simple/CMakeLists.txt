add_library(test-wasm-simple STATIC "${CMAKE_CURRENT_LIST_DIR}/main.cpp")

target_link_libraries(test-wasm-simple PUBLIC cui)
set_target_properties(test-wasm-simple PROPERTIES FOLDER "test")
