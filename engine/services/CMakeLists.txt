include(ntt-config)
ntt_config()

set(NTT_SERVICES_LIBRARY_NAME "ntt-services")
ntt_project(NTT_SERVICES_LIBRARY_NAME)

ntt_set_source_group("src"
    NTT_SERVICES_SOURCES
    "services.h"
)

ntt_set_source_group("src/logging"
    NTT_LOGGING_SOURCES
    "logging/logging.h"
    "logging/logging.cpp"
    "logging/logging_level.h"
    "logging/logging_level.cpp"
    "logging/logging_type.h"
    "logging/logging_type.cpp"
    "logging/logging_message.h"
    "logging/logging_message.cpp"
)

ntt_set_source_group("src/logging/handlers"
    NTT_LOGGING_HANDLERS_SOURCES
    "logging/handlers/handlers.h"
    "logging/handlers/handlers.cpp"
    "logging/handlers/console_handler.h"
    "logging/handlers/console_handler.cpp"
)

ntt_set_source_group("defs/logging"
    NTT_LOGGING_DEFS_SOURCES
    "logging/logging_type.def"
    "logging/logging_level.def"
)

add_library(
    ${NTT_SERVICES_LIBRARY_NAME}
    ${NTT_SERVICES_SOURCES}
    ${NTT_LOGGING_SOURCES}
    ${NTT_LOGGING_DEFS_SOURCES}
)

target_include_directories(
    ${NTT_SERVICES_LIBRARY_NAME}
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

ntt_find_package(ntt-glfw REQUIRED)

target_link_libraries(
    ${NTT_SERVICES_LIBRARY_NAME}
    PUBLIC
    ntt-core
    ntt-glfw
)

ntt_add_options(${NTT_SERVICES_LIBRARY_NAME})
ntt_add_link_flags(${NTT_SERVICES_LIBRARY_NAME})
ntt_add_compile_options(${NTT_SERVICES_LIBRARY_NAME})
