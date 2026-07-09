set(STB_LIBRARY_NAME ntt-stb)

if (TARGET ${STB_LIBRARY_NAME})
    return()
endif()

include(FetchContent)

FetchContent_Declare(
    stb
    GIT_REPOSITORY
    https://github.com/threezinedine/stb.git
    SOURCE_DIR
    "${CMAKE_SOURCE_DIR}/externals/stb"
)

FetchContent_MakeAvailable(stb)

add_library(${STB_LIBRARY_NAME} INTERFACE)

target_link_libraries(${STB_LIBRARY_NAME} INTERFACE stb)