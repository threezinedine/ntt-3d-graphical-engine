include(ntt-config)
ntt_config()

if (NTT_EDITOR)
    ntt_set_source_group(
        "src"
        NTT_EDITOR_SOURCES
        "main_editor.cpp"
    )

    add_executable(
        ntt-editor
        ${NTT_EDITOR_SOURCES}
    )

    target_link_libraries(
        ntt-editor
        PRIVATE
        ntt-core
        ntt-services
    )

    ntt_add_options(ntt-editor)
    ntt_add_link_flags(ntt-editor)
    ntt_add_compile_options(ntt-editor)

    if (NTT_PLATFORM_WEB)
        add_custom_command(
            TARGET ntt-editor POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${CMAKE_SOURCE_DIR}/engine/assets/html/editor.html
                ${CMAKE_CURRENT_BINARY_DIR}/index.html
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${CMAKE_SOURCE_DIR}/engine/assets/icons/logo.ico
                ${CMAKE_CURRENT_BINARY_DIR}/favicon.ico
        )
    endif()
endif()
