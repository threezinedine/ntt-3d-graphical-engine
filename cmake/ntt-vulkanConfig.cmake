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

FetchContent_Declare(
    spirv-reflect
    GIT_REPOSITORY
    https://github.com/KhronosGroup/SPIRV-Reflect.git
    GIT_TAG main
    SOURCE_DIR
    "${CMAKE_SOURCE_DIR}/externals/SPIRV-Reflect"
)

set(SPIRV_REFLECT_EXAMPLES OFF)
set(SPIRV_REFLECT_EXECUTABLE OFF)
set(SPIRV_REFLECT_INSTALL OFF)
set(SPIRV_REFLECT_STATIC_LIB ON)
set(SPIRV_REFLECT_BUILD_TESTS OFF)
FetchContent_MakeAvailable(spirv-reflect)

target_link_libraries(
    ${VULKAN_TARGET}
    INTERFACE 
    Vulkan::Vulkan
    glslang::glslang
    glslang::glslang-default-resource-limits
    spirv-reflect-static
)