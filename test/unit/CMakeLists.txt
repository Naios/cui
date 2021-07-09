file(
  GLOB SOURCES
  LIST_DIRECTORIES false
  CONFIGURE_DEPENDS ${CMAKE_CURRENT_LIST_DIR}/*.cpp)

foreach(TEST_SOURCE IN LISTS SOURCES)
  get_filename_component(FILE_NAME "${TEST_SOURCE}" NAME_WE)

  set(PROJECT_NAME ${FILE_NAME})
  set(TEST_NAME ${FILE_NAME}-unit-test)

  add_executable(${PROJECT_NAME} ${TEST_SOURCE})
  target_link_libraries(${PROJECT_NAME} PUBLIC cui Catch2::Catch2)
  target_compile_definitions(${PROJECT_NAME} PUBLIC CUI_HAS_PEDANTIC_ASSERT
                                                    CATCH_CONFIG_MAIN)
  set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "test/unit")

  add_test(NAME ${TEST_NAME} COMMAND ${PROJECT_NAME})
endforeach()
