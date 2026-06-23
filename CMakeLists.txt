cmake_minimum_required(VERSION 4.0)

project(ntt-3d-graphical-engine)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

include(ntt-config)

ntt_config()
ntt_platform_detect()

add_subdirectory(engine)