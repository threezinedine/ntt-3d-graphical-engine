include(ntt-assert)

macro(ntt_config)
    if (NOT NTT_CONFIGURED)
        set(NTT_OPTIONS)
        set(NTT_LINK_FLAGS)
        set(NTT_COMPILE_OPTIONS)

        if (MSVC)
            set_property(GLOBAL PROPERTY USE_FOLDERS ON)
            list(APPEND NTT_OPTIONS NTT_MSVC)
            set(NTT_MSVC ON)
        endif()

        option(CMAKE_BUILD_TYPE "Build type" Debug)
        ntt_option(NTT_ENABLE_ASSERTION ON)

        ntt_option(NTT_EDITOR ON)
        ntt_option(NTT_ENGINE ON)
        ntt_option(NTT_UNITTEST OFF)

        ntt_assert_max_one_is_on(NTT_ENGINE NTT_UNITTEST)
        ntt_assert_max_one_is_on(NTT_EDITOR NTT_UNITTEST)
        ntt_assert_at_least_one_is_on(NTT_EDITOR NTT_ENGINE)

        if (CMAKE_BUILD_TYPE STREQUAL "Debug")
            set(NTT_DEBUG ON)
            list(APPEND NTT_OPTIONS NTT_DEBUG)
        elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
            set(NTT_RELEASE ON)
            list(APPEND NTT_OPTIONS NTT_RELEASE)
        else()
            message(FATAL_ERROR "Unsupported build type: ${CMAKE_BUILD_TYPE}")
        endif()

        ntt_platform_detect()
        set(NTT_CONFIGURED ON)
    endif()
endmacro()

macro(ntt_platform_detect)
    if (NOT NTT_PLATFORM_DETECTED)
        if (EMSCRIPTEN)
            set(NTT_PLATFORM_WEB ON)
            list(APPEND NTT_OPTIONS NTT_PLATFORM_WEB)
        elseif (WIN32)
            set(NTT_PLATFORM_WINDOWS ON)
            list(APPEND NTT_OPTIONS NTT_PLATFORM_WINDOWS)
            list(APPEND NTT_COMPILE_OPTIONS
                "/W4"
                "/WX"
                "/permissive-")
        elseif (APPLE)
            message(FATAL_ERROR "Apple platform is not supported yet")
        elseif (UNIX)
            set(NTT_PLATFORM_UNIX ON)
            list(APPEND NTT_OPTIONS NTT_PLATFORM_UNIX)
            list(APPEND 
                NTT_COMPILE_OPTIONS 
                "-Werror" 
                "-Wall" 
                "-Wextra" 
                "-Wpedantic")

            if (NTT_DEBUG)
                list(APPEND NTT_COMPILE_FLAGS "-g" "-fno-omit-frame-pointer" "-O0")
                list(APPEND NTT_LINK_FLAGS "-rdynamic" "-no-pie")
            endif()
        else()
            message(FATAL_ERROR "Unsupported platform")
        endif()

        set(NTT_PLATFORM_DETECTED ON)
    endif()
endmacro()

macro(ntt_print_options)
    message(STATUS "OPTIONS:")
    foreach(option IN LISTS NTT_OPTIONS)
        if (${${option}} STREQUAL "ON")
            message("        ${option}=1")
        elseif (${${option}} STREQUAL "OFF")
            message("        ${option}=0")
        else()
            message("        ${option}=${${option}}")
        endif()
    endforeach()
endmacro()

macro(ntt_add_options target)
    foreach(option IN LISTS NTT_OPTIONS)
        if (${${option}} STREQUAL "ON")
            target_compile_definitions(${target} PUBLIC ${option}=1)
        elseif (${${option}} STREQUAL "OFF")
            target_compile_definitions(${target} PUBLIC ${option}=0)
        else()
            target_compile_definitions(${target} PUBLIC ${option}=${${option}})
        endif()
    endforeach()
endmacro()

macro(ntt_print_link_flags)
    message(STATUS "LINK FLAGS:")
    foreach(flag IN LISTS NTT_LINK_FLAGS)
        message("        ${flag}")
    endforeach()
endmacro()

macro(ntt_add_link_flags target)
    foreach(flag IN LISTS NTT_LINK_FLAGS)
        target_link_options(${target} PUBLIC ${flag})
    endforeach()
endmacro()

macro(ntt_print_compile_options)
    message(STATUS "COMPILE OPTIONS:")
    foreach(option IN LISTS NTT_COMPILE_OPTIONS)
        message("        ${option}")
    endforeach()
endmacro()

macro(ntt_add_compile_options target)
    foreach(option IN LISTS NTT_COMPILE_OPTIONS)
        target_compile_options(${target} PUBLIC ${option})
    endforeach()
endmacro()

macro(ntt_option option default_value)
    option(${option} "Option ${option}" ${default_value})
    list(APPEND NTT_OPTIONS ${option})
endmacro()

macro(ntt_set_source_group source_group_name variable)
    set(${variable} ${ARGN})

    if (MSVC)
        source_group(${source_group_name} FILES ${ARGN})
    endif()
endmacro()