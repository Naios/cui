add_executable(vm main.cpp)
target_link_libraries(vm PUBLIC cui)

set_target_properties(vm PROPERTIES FOLDER "tools")
