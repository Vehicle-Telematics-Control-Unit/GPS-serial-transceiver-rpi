set(BUILD_LIB_NAME TinyGPS)
cmake_minimum_required(VERSION 3.22)
add_library(${BUILD_LIB_NAME} SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/src/TinyGPS++.cpp
)