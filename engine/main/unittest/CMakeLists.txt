include(ntt-config)
ntt_config()

if (NTT_UNITTEST)
    set(NTT_UNITTEST_EXECUTABLE_NAME "ntt-unittest")
    ntt_project(NTT_UNITTEST_EXECUTABLE_NAME)

    ntt_set_source_group(
        "src/utilities"
        NTT_UTILITIES_SOURCES
        "utilities/utilities.h"
        "utilities/utilities.cpp"
    )

    ntt_set_source_group(
        "src/tests/core"
        NTT_CORE_TESTS_SOURCES
        "tests/core/test_malloc.cpp"
        "tests/core/test_string.cpp"
        "tests/core/test_smart_ptr.cpp"
        "tests/core/container_test_object.h"
        "tests/core/test_id.cpp"
    )

    ntt_set_source_group(
        "src/tests/containers"
        NTT_CONTAINERS_TESTS_SOURCES
        "tests/core/containers/test_array.cpp"
        "tests/core/containers/test_list.cpp"
        "tests/core/containers/test_stack.cpp"
        "tests/core/containers/test_queue.cpp"
        "tests/core/containers/test_storage.cpp"
        "tests/core/containers/test_map.cpp"
    )

    ntt_set_source_group(
        "src/tests/application"
        NTT_APPLICATION_TESTS_SOURCES
        "tests/applications/test_object.cpp"
    )

    ntt_set_source_group(
        "src/tests/services"
        NTT_SERVICES_TESTS_SOURCES
        "tests/services/test_logging.cpp"
    )

    ntt_set_source_group(
        "src"
        NTT_UNITTEST_SOURCES
        "main_unittest.cpp"
    )

    add_executable(
        ${NTT_UNITTEST_EXECUTABLE_NAME}
        ${NTT_UNITTEST_SOURCES}
        ${NTT_UTILITIES_SOURCES}
        ${NTT_CORE_TESTS_SOURCES}
        ${NTT_CONTAINERS_TESTS_SOURCES}
        ${NTT_SERVICES_TESTS_SOURCES}
        ${NTT_APPLICATION_TESTS_SOURCES}
    )

    target_link_libraries(
        ${NTT_UNITTEST_EXECUTABLE_NAME}
        PRIVATE
        ntt-application
    )

    target_include_directories(
        ${NTT_UNITTEST_EXECUTABLE_NAME}
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
    )

    ntt_add_options(${NTT_UNITTEST_EXECUTABLE_NAME})
    ntt_add_link_flags(${NTT_UNITTEST_EXECUTABLE_NAME})
    ntt_add_compile_options(${NTT_UNITTEST_EXECUTABLE_NAME})

    if (NTT_PLATFORM_WEB)
        add_custom_command(
            TARGET ${NTT_UNITTEST_EXECUTABLE_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${CMAKE_SOURCE_DIR}/engine/assets/html/engine.html
                ${CMAKE_CURRENT_BINARY_DIR}/index.html
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${CMAKE_SOURCE_DIR}/engine/assets/icons/logo.ico
                ${CMAKE_CURRENT_BINARY_DIR}/favicon.ico
        )
    endif()
endif()
