if(${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.16)
  option(${PROJECT_NAME}_WITH_PCH "Enable the precompiled header compilation"
         ON)
endif()

add_subdirectory(cui)
add_subdirectory(unity)
add_subdirectory(viewer)
