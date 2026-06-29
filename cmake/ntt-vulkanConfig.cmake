set(VULKAN_TARGET ntt-vulkan)

if (TARGET ${VULKAN_TARGET})
    return()
endif()

find_package(Vulkan REQUIRED)

add_library(${VULKAN_TARGET} INTERFACE IMPORTED)

target_link_libraries(
    ${VULKAN_TARGET}
    INTERFACE 
    Vulkan::Vulkan
)