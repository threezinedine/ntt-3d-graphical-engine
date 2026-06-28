set(TARGET_NAME "ntt-glfw")

if (TARGET ${TARGET_NAME})
    return()
endif()

add_library(${TARGET_NAME} INTERFACE)

if (NOT NTT_PLATFORM_WEB)
    include(FetchContent)

    FetchContent_Declare(
        glfw
        GIT_REPOSITORY
        https://github.com/glfw/glfw.git
        SOURCE_DIR
        "${CMAKE_SOURCE_DIR}/externals/glfw"
    )

    FetchContent_MakeAvailable(glfw)

    FetchContent_Declare(
        glad
        GIT_REPOSITORY
        https://github.com/threezinedine/glad.git
        SOURCE_DIR
        "${CMAKE_SOURCE_DIR}/externals/glad"
    )

    FetchContent_MakeAvailable(glad)

    target_link_libraries(${TARGET_NAME} INTERFACE glfw glad)
endif()