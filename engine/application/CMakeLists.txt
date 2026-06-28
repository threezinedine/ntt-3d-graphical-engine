include(ntt-config)
ntt_config()

set(NTT_APPLICATION_LIBRARY_NAME "ntt-application")
ntt_project(NTT_APPLICATION_LIBRARY_NAME)

ntt_set_source_group("src"
    NTT_APPLICATION_SOURCES
    "applications.h"
    "applications.cpp"
    "ref_counted.h"
    "ref_counted.cpp"
    "object.h"
    "object.cpp"
    "application.h"
    "application.cpp"
)

ntt_set_source_group("src/systems"
    NTT_SYSTEMS_SOURCES
    "systems/systems.h"
    "systems/systems.cpp"
    "systems/system.h"
    "systems/system.cpp"
    "systems/system_globals.h"
    "systems/system_globals.cpp"
)

ntt_set_source_group("src/systems/display"
    NTT_DISPLAY_SYSTEM_SOURCES
    "systems/display/display.h"
    "systems/display/display_system.h"
    "systems/display/display_system.cpp"
)

add_library(
    ${NTT_APPLICATION_LIBRARY_NAME}
    STATIC
    ${NTT_APPLICATION_SOURCES}
    ${NTT_SYSTEMS_SOURCES}
    ${NTT_DISPLAY_SYSTEM_SOURCES}
)

target_include_directories(
    ${NTT_APPLICATION_LIBRARY_NAME}
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(
    ${NTT_APPLICATION_LIBRARY_NAME}
    PUBLIC
    ntt-services
)

ntt_add_options(${NTT_APPLICATION_LIBRARY_NAME})
ntt_add_link_flags(${NTT_APPLICATION_LIBRARY_NAME})
ntt_add_compile_options(${NTT_APPLICATION_LIBRARY_NAME})