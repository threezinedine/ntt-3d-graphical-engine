set(VULKAN_TARGET ntt-vulkan)

if (TARGET ${VULKAN_TARGET})
    return()
endif()

set(ALLOW_EXTERNAL_SPIRV_TOOLS FALSE)
set(ENABLE_OPT OFF)
find_package(Vulkan REQUIRED)

add_library(${VULKAN_TARGET} INTERFACE IMPORTED)

include (FetchContent)

FetchContent_Declare(
    glslang
    GIT_REPOSITORY
    https://github.com/KhronosGroup/glslang.git
    GIT_TAG main
    SOURCE_DIR
    "${CMAKE_SOURCE_DIR}/externals/glslang"
)

FetchContent_MakeAvailable(glslang)

target_link_libraries(
    ${VULKAN_TARGET}
    INTERFACE 
    Vulkan::Vulkan
    glslang::glslang
    glslang::glslang-default-resource-limits
)