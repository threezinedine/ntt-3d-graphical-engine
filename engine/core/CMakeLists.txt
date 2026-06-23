include(ntt-config)
ntt_config()

set(NTT_CORE_LIBRARY_NAME "ntt-core")

set(NTT_ALLOC_SOURCES
    "alloc/iallocator.h"
    "alloc/alloc.h"
)

if (MSVC)
    source_group("src/alloc" FILES ${NTT_ALLOC_SOURCES})
endif()

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

if (MSVC)
    source_group("src" FILES ${NTT_CORE_SOURCES})
endif()

add_library(
    ${NTT_CORE_LIBRARY_NAME}
    ${NTT_CORE_SOURCES}
    ${NTT_ALLOC_SOURCES}
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