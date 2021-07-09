add_library(
  adafruit STATIC
  "${CMAKE_CURRENT_LIST_DIR}/adafruit/Adafruit_GFX.h"
  "${CMAKE_CURRENT_LIST_DIR}/adafruit/Adafruit_GFX.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/adafruit/gfxfont.h"
  "${CMAKE_CURRENT_LIST_DIR}/adafruit/glcdfont.c"
  "${CMAKE_CURRENT_LIST_DIR}/polyfill/Arduino.h"
  "${CMAKE_CURRENT_LIST_DIR}/polyfill/Print.h")

target_compile_definitions(adafruit PUBLIC ARDUINO=100)

target_include_directories(adafruit PUBLIC "${CMAKE_CURRENT_LIST_DIR}/adafruit"
                                           "${CMAKE_CURRENT_LIST_DIR}/polyfill")

set_target_properties(adafruit PROPERTIES FOLDER "dep")
