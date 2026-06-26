include(ntt-config)
ntt_config()

set(NTT_SERVICES_LIBRARY_NAME "ntt-services")

ntt_set_source_group("src"
    NTT_SERVICES_SOURCES
    "services.h"
    "ref_counted.h"
    "ref_counted.cpp"
    "object.h"
    "object.cpp"
    "application.h"
    "application.cpp"
)

add_library(
    ${NTT_SERVICES_LIBRARY_NAME}
    ${NTT_SERVICES_SOURCES}
)

target_include_directories(
    ${NTT_SERVICES_LIBRARY_NAME}
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(
    ${NTT_SERVICES_LIBRARY_NAME}
    PUBLIC
    ntt-core
)

ntt_add_options(${NTT_SERVICES_LIBRARY_NAME})
ntt_add_link_flags(${NTT_SERVICES_LIBRARY_NAME})
ntt_add_compile_options(${NTT_SERVICES_LIBRARY_NAME})