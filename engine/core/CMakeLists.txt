include(ntt-config)
ntt_config()

set(NTT_CORE_LIBRARY_NAME "ntt-core")

set(
    NTT_CORE_SOURCES
    "core.h"
    "defs.h"
    "common.h"
    "result.h"
    "result.cpp"
    "console.h"
    "console.cpp"
)

add_library(
    ${NTT_CORE_LIBRARY_NAME}
    ${NTT_CORE_SOURCES}
)

target_include_directories(
    ${NTT_CORE_LIBRARY_NAME}
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_precompile_headers(
    ${NTT_CORE_LIBRARY_NAME}
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/common.h"
)

ntt_add_options(${NTT_CORE_LIBRARY_NAME})
ntt_add_link_flags(${NTT_CORE_LIBRARY_NAME})
ntt_add_compile_options(${NTT_CORE_LIBRARY_NAME})