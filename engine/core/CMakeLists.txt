include(ntt-config)
ntt_config()

set(NTT_CORE_LIBRARY_NAME "ntt-core")

set(
    NTT_CORE_SOURCES
    "core.h"
    "core.cpp"
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