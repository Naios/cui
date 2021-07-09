set(BUILD_SHARED_LIBS OFF)
set(FMT_DOC OFF)
set(FMT_TEST OFF)

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/fmt" EXCLUDE_FROM_ALL)

set_target_properties(fmt PROPERTIES FOLDER "dep")
