cmake_minimum_required(VERSION 3.7)
project(curves VERSION 0.0.1 DESCRIPTION "library for curves")

set(CXXFLAGS -Wall -Wextra -Wpedantic)

file(GLOB_RECURSE SRC_LIST src/*.cxx)

set(CMAKE_CXX_STANDARD 23)
add_compile_options($ENV{CXXFLAGS} ${CXXFLAGS})

add_library(${PROJECT_NAME} SHARED ${SRC_LIST})

target_include_directories(${PROJECT_NAME} PRIVATE . src include)

set_target_properties(${PROJECT_NAME} PROPERTIES
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
  PUBLIC_HEADER include/curves.hxx
)
