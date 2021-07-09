# set(BUILD_SHARED_LIBS OFF) set(BUILD_WASI none)

file(
  GLOB SOURCES
  LIST_DIRECTORIES false
  CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/wasm3/source/*.c"
  "${CMAKE_CURRENT_LIST_DIR}/wasm3/source/*.h")

add_library(m3 STATIC ${SOURCES})

target_include_directories(m3 PUBLIC "${CMAKE_CURRENT_LIST_DIR}/wasm3/source")

target_compile_definitions(m3 PUBLIC d_m3HasWASI d_m3SkipMemoryBoundsCheck)

set_target_properties(m3 PROPERTIES FOLDER "dep")
