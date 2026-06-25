include(ntt-config)
ntt_config()

if (NTT_UNITTEST)
    ntt_set_source_group(
        "src/utilities"
        NTT_UTILITIES_SOURCES
        "utilities/utilities.h"
        "utilities/utilities.cpp"
    )

    ntt_set_source_group(
        "src/tests"
        NTT_TESTS_SOURCES
        "tests/test_malloc.cpp"
        "tests/test_string.cpp"
    )

    ntt_set_source_group(
        "src/tests/containers"
        NTT_CONTAINERS_SOURCES
        "tests/containers/container_test_object.h"
        "tests/containers/test_array.cpp"
        "tests/containers/test_list.cpp"
        "tests/containers/test_stack.cpp"
        "tests/containers/test_queue.cpp"
    )

    ntt_set_source_group(
        "src"
        NTT_UNITTEST_SOURCES
        "main_unittest.cpp"
    )

    add_executable(
        ntt-unittest
        ${NTT_UNITTEST_SOURCES}
        ${NTT_UTILITIES_SOURCES}
        ${NTT_TESTS_SOURCES}
        ${NTT_CONTAINERS_SOURCES}
    )

    target_link_libraries(
        ntt-unittest
        PRIVATE
        ntt-core
    )

    target_include_directories(
        ntt-unittest
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
    )

    ntt_add_options(ntt-unittest)
    ntt_add_link_flags(ntt-unittest)
    ntt_add_compile_options(ntt-unittest)

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
