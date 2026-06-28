include(ntt-config)
ntt_config()

if (NTT_EDITOR)
    set(NTT_EDITOR_EXECUTABLE_NAME "ntt-editor")
    ntt_project(NTT_EDITOR_EXECUTABLE_NAME)

    set(NTT_EDITOR_SOURCES)

    ntt_set_source_group(
        "src"
        NTT_EDITOR_SOURCES
        "main_editor.cpp"
    )

    add_executable(
        ${NTT_EDITOR_EXECUTABLE_NAME}
        ${NTT_EDITOR_SOURCES}
    )

    target_link_libraries(
        ${NTT_EDITOR_EXECUTABLE_NAME}
        PRIVATE
        ntt-application
    )

    ntt_add_options(${NTT_EDITOR_EXECUTABLE_NAME})
    ntt_add_link_flags(${NTT_EDITOR_EXECUTABLE_NAME})
    ntt_add_compile_options(${NTT_EDITOR_EXECUTABLE_NAME})

    if (NTT_PLATFORM_WEB)
        add_custom_command(
            TARGET ${NTT_EDITOR_EXECUTABLE_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${CMAKE_SOURCE_DIR}/engine/assets/html/editor.html
                ${CMAKE_CURRENT_BINARY_DIR}/index.html
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${CMAKE_SOURCE_DIR}/engine/assets/icons/logo.ico
                ${CMAKE_CURRENT_BINARY_DIR}/favicon.ico
        )
    endif()
endif()
