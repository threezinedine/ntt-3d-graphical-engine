include(ntt-config)
ntt_config()

set(NTT_CORE_LIBRARY_NAME "ntt-core")
ntt_project(NTT_CORE_LIBRARY_NAME)

set(NTT_CORE_SOURCES)

ntt_set_source_group("src/containers"
    NTT_CORE_SOURCES
    "containers/containers.h"
    "containers/array.h"
    "containers/list.h"
    "containers/stack.h"
    "containers/queue.h"
    "containers/storage.h"
    "containers/map.h"
)

ntt_set_source_group("defs"
    NTT_CORE_SOURCES
    "console_color.def"
    "result.def"
    "id.def"
    "argparser/arg_type.def"
    "argparser/arg_options.def"
)

ntt_set_source_group("src/argparser"
    NTT_CORE_SOURCES
    "argparser/argparser.h"
    "argparser/argparser.cpp"
)

ntt_set_source_group("src/utils"
    NTT_CORE_SOURCES
    "utils/utils.h"
    "utils/utils.cpp"
)

ntt_set_source_group("src/alloc" 
    NTT_CORE_SOURCES
    "alloc/iallocator.h"
    "alloc/alloc.h"
    "alloc/alloc.cpp"
    "alloc/malloc_allocator.h"
    "alloc/malloc_allocator.cpp"
    "alloc/stack_allocator.h"
    "alloc/stack_allocator.cpp"
)

ntt_set_source_group("src/backtrace"
    NTT_CORE_SOURCES
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
    "str.h"
    "str.cpp"
    "id.h"
    "id.cpp"
)

ntt_set_source_group("src/smart_ptr"
    NTT_CORE_SOURCES
    "smart_ptr/smart_ptr.h"
    "smart_ptr/scope.h"
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