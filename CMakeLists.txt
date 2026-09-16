cmake_minimum_required(VERSION 3.16)

# ESP-DL 3.3.x references MALLOC_CAP_SIMD, which is unavailable in IDF 5.3.
# WakeNet does not use the SIMD-specific image path, so fall back to default heap.
add_compile_definitions(MALLOC_CAP_SIMD=0)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(xiao_147_voice_assistant)
