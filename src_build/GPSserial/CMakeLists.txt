set(BUILD_LIB_NAME gps_serial)
cmake_minimum_required(VERSION 3.22)
include_directories(../dependencies/TinyGPSPlus_/src ../dependencies/someipService ../dependencies/nlohmann ../)
add_library(${BUILD_LIB_NAME} SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/GPSserial.cpp
)