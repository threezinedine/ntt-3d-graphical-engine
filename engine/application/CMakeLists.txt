include(ntt-config)
ntt_config()

set(NTT_APPLICATION_LIBRARY_NAME "ntt-application")
ntt_project(NTT_APPLICATION_LIBRARY_NAME)

ntt_set_source_group("src"
    NTT_APPLICATION_SOURCES
    "applications.h"
    "ref_counted.h"
    "ref_counted.cpp"
    "object.h"
    "object.cpp"
    "application.h"
    "application.cpp"
)

add_library(
    ${NTT_APPLICATION_LIBRARY_NAME}
    ${NTT_APPLICATION_SOURCES}
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