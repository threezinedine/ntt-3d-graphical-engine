include(ntt-config)
ntt_config()

set(NTT_CORE_LIBRARY_NAME "ntt-core")

ntt_set_source_group("src/containers"
    NTT_CONTAINERS_SOURCES
    "containers/containers.h"
    "containers/array.h"
)

ntt_set_source_group("src/alloc" 
    NTT_ALLOC_SOURCES
    "alloc/iallocator.h"
    "alloc/alloc.h"
    "alloc/alloc.cpp"
    "alloc/malloc_allocator.h"
    "alloc/malloc_allocator.cpp"
)

ntt_set_source_group("src/backtrace"
    NTT_BACKTRACE_SOURCES
    "backtrace/backtrace.h"
    "backtrace/backtrace.cpp"
)

ntt_set_source_group("src/core"
    NTT_CORE_SOURCES
    "core.h"
    "defs.h"
    "raii.h"
    "common.h"
    "result.h"
    "result.cpp"
    "console.h"
    "console.cpp"
    "globals.h"
    "globals.cpp"
)

add_library(
    ${NTT_CORE_LIBRARY_NAME}
    ${NTT_CORE_SOURCES}
    ${NTT_ALLOC_SOURCES}
    ${NTT_BACKTRACE_SOURCES}
    ${NTT_CONTAINERS_SOURCES}
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