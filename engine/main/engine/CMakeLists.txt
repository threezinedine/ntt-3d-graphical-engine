include(ntt-config)
ntt_config()

if (NTT_ENGINE)
    ntt_set_source_group(
        "src"
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
        ntt-services
    )

    ntt_add_options(ntt-engine)
    ntt_add_link_flags(ntt-engine)
    ntt_add_compile_options(ntt-engine)

    if (NTT_PLATFORM_WEB)
        add_custom_command(
            TARGET ntt-engine POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${CMAKE_SOURCE_DIR}/engine/assets/html/engine.html
                ${CMAKE_CURRENT_BINARY_DIR}/index.html
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${CMAKE_SOURCE_DIR}/engine/assets/icons/logo.ico
                ${CMAKE_CURRENT_BINARY_DIR}/favicon.ico
        )
    endif()
endif()