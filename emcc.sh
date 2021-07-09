#!/bin/bash -e
PROJECT_DIR=$(dirname "$0")
OUT_FILE='index.wasm'

# -s EXPORTED_RUNTIME_METHODS='["_cui_surface_changed","_cui_surface_begin","_cui_surface_end","_cui_surface_flush"]' \
# -s MAIN_MODULE=2 \
# -s SUPPORT_LONGJMP=0 \

# -s JS_MATH=1 \
# -s POLYFILL_OLD_MATH_FUNCTIONS=0 \
# -Dcui_EXPORTS \
# # -s DISABLE_EXCEPTION_CATCHING=1 \
# -DCUI_HAS_HOST_MATH=1 \

# MALLOC = "dlmalloc"; # emmalloc
# -s USES_DYNAMIC_ALLOC=0 \

# Compile the .wasm to fit into one wasm memory page (`(memory (;0;) 1 1)`)
# Otherwise this will blow up the ESP32 heap
em++ $@ $PROJECT_DIR/lib/unity/unity.cpp \
        $PROJECT_DIR/lib/cui/surface/vm/host.cpp \
        $PROJECT_DIR/lib/wasm/noalloc.cpp \
        -s WASM=1 \
        -s EVAL_CTORS=1 \
        -s STANDALONE_WASM --no-entry \
        -s TOTAL_STACK=2048 \
        -s INITIAL_MEMORY=65536 \
        -s EXPORTED_FUNCTIONS='["_setup","_loop"]' \
        -s ERROR_ON_UNDEFINED_SYMBOLS=0 \
        -s SUPPORT_LONGJMP=0 \
        -s ALLOW_MEMORY_GROWTH=0 \
        -DCUI_HAS_NO_ALLOCATOR=1 \
        -s USES_DYNAMIC_ALLOC=0 \
        -s JS_MATH=1 \
        -s POLYFILL_OLD_MATH_FUNCTIONS=0 \
        -s DISABLE_EXCEPTION_CATCHING=1 \
        -s DISABLE_EXCEPTION_THROWING=1 \
        -s LEGALIZE_JS_FFI=0 \
        -s TEXTDECODER=0 \
        -s SUPPORT_ERRNO=0 \
        -DWASM=1 \
        -DCUI_HAS_DYNAMIC_LINKING=1 \
        -DNDEBUG=1 \
        -Os \
        -g0 \
        -flto=full \
        -fno-rtti \
        -fno-exceptions \
        -I$PROJECT_DIR/include \
        -I$PROJECT_DIR/lib \
        -std=c++17 \
        -o $OUT_FILE

# https://unix.stackexchange.com/a/16645
file_size_kb=`du -b "$OUT_FILE" | cut -f1`

echo "---"
echo "Bytes: ${file_size_kb}"


wasm2wat $OUT_FILE > index.wat

# wasm-objdump -x -j Data a.out.wasm
