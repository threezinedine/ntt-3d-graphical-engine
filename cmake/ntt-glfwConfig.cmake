set(TARGET_NAME "ntt-glfw")

if (TARGET ${TARGET_NAME})
    return()
endif()

include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY
    https://github.com/glfw/glfw.git
    SOURCE_DIR
    "${CMAKE_SOURCE_DIR}/externals"
)

FetchContent_MakeAvailable(glfw)

add_library(${TARGET_NAME} INTERFACE)

target_link_libraries(${TARGET_NAME} INTERFACE glfw)