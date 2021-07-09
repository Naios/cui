add_executable(main main.cpp)
target_link_libraries(main PUBLIC cui viewer)

set_target_properties(main PROPERTIES FOLDER "tools")
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT main)
