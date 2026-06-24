cmake_minimum_required(VERSION 4.0)

project(ntt-3d-graphical-engine)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

include(ntt-config)

ntt_config()

ntt_print_options()
ntt_print_link_flags()
ntt_print_compile_options()

add_subdirectory(engine)

if (MSVC)
    if (NTT_ENGINE)
        set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ntt-engine)
    elseif (NTT_EDITOR)
        set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ntt-editor)
    elseif (NTT_UNITTEST)
        set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ntt-unittest)
    endif()
endif()