cmake_minimum_required(VERSION 3.18)

project(ft232gpio CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(PkgConfig REQUIRED)
pkg_check_modules(FTDI1 IMPORTED_TARGET libftdi1)
if(NOT ${FTDI1_FOUND})
  mesage(FATAL "libftdi1 not found")
endif()

message(STATUS "FTDI1_INCLUDE_DIRS=${FTDI1_INCLUDE_DIRS}")
message(STATUS "FTDI1_LIBRARIES=${FTDI1_LIBRARIES}")
message(STATUS "FTDI1_LINK_LIBRARIES=${FTDI1_LINK_LIBRARIES}")

add_subdirectory(ft232gpio)
