# The Flutter tooling requires that developers have CMake 3.10 or later
# installed. You should not increase this version, as doing so will cause
# the plugin to fail to compile for some customers of the plugin.
cmake_minimum_required(VERSION 3.10)

project(reversi_library VERSION 0.0.1 LANGUAGES CXX)
set (CMAKE_CXX_STANDARD 11)

add_library(reversi SHARED
  reversi.cpp
)

set_target_properties(reversi PROPERTIES
  PUBLIC_HEADER reversi.h
  OUTPUT_NAME "reversi"
)

target_compile_definitions(reversi PUBLIC DART_SHARED_LIB)

if (ANDROID)
  # Support Android 15 16k page size
  target_link_options(reversi PRIVATE "-Wl,-z,max-page-size=16384")
endif()
