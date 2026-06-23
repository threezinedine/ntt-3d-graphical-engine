include(ntt-config)
ntt_config()

set(
    NTT_ENGINE_SOURCES
    "main_engine.cpp"
)

add_executable(
    ntt-engine
    ${NTT_ENGINE_SOURCES}
)

target_link_libraries(
    ntt-engine
    PRIVATE
    ntt-core
)

if (NTT_PLATFORM_WEB)
    add_custom_command(
        TARGET ntt-engine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/engine/assets/html/engine.html
            ${CMAKE_CURRENT_BINARY_DIR}/index.html
    )
endif()