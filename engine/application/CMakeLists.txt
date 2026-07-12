include(ntt-config)
ntt_config()

set(NTT_APPLICATION_LIBRARY_NAME "ntt-application")
ntt_project(NTT_APPLICATION_LIBRARY_NAME)

set(NTT_APPLICATION_SOURCES)

ntt_set_source_group("src"
    NTT_APPLICATION_SOURCES
    "applications.h"
    "applications.cpp"
    "ref_counted.h"
    "ref_counted.cpp"
    "object.h"
    "object.cpp"
    "application.h"
    "application.cpp"
    "ecs.h"
    "ecs.cpp"
)

ntt_set_source_group("src/drivers"
    NTT_APPLICATION_SOURCES
    "drivers/drivers.h"
)

ntt_set_source_group("src/drivers/ntt-glfw"
    NTT_APPLICATION_SOURCES
    "drivers/ntt_glfw/ntt_glfw.h"
    "drivers/ntt_glfw/ntt_glfw_driver.h"
    "drivers/ntt_glfw/ntt_glfw_driver.cpp"
)

ntt_set_source_group("src/drivers/ntt-opengl"
    NTT_APPLICATION_SOURCES
    "drivers/ntt_opengl/ntt_opengl_inc.h"
    "drivers/ntt_opengl/ntt_opengl.h"
    "drivers/ntt_opengl/ntt_opengl.cpp"
    "drivers/ntt_opengl/ntt_opengl_mesh_storage.h"
    "drivers/ntt_opengl/ntt_opengl_mesh_storage.cpp"
    "drivers/ntt_opengl/ntt_opengl_mesh_view_storage.h"
    "drivers/ntt_opengl/ntt_opengl_mesh_view_storage.cpp"
    "drivers/ntt_opengl/ntt_opengl_texture_storage.h"
    "drivers/ntt_opengl/ntt_opengl_texture_storage.cpp"
    "drivers/ntt_opengl/ntt_opengl_shader_storage.h"
    "drivers/ntt_opengl/ntt_opengl_shader_storage.cpp"
)

ntt_set_source_group("src/drivers/ntt-vulkan"
    NTT_APPLICATION_SOURCES
    "drivers/ntt_vulkan/ntt_vulkan_inc.h"
    "drivers/ntt_vulkan/ntt_vulkan_inc.cpp"
    "drivers/ntt_vulkan/ntt_vulkan.h"
    "drivers/ntt_vulkan/ntt_vulkan.cpp"
    "drivers/ntt_vulkan/ntt_vulkan_mesh_storage.h"
    "drivers/ntt_vulkan/ntt_vulkan_mesh_storage.cpp"
    "drivers/ntt_vulkan/ntt_vulkan_mesh_view_storage.h"
    "drivers/ntt_vulkan/ntt_vulkan_mesh_view_storage.cpp"
    "drivers/ntt_vulkan/ntt_vulkan_texture_storage.h"
    "drivers/ntt_vulkan/ntt_vulkan_texture_storage.cpp"
    "drivers/ntt_vulkan/ntt_vulkan_shader_storage.h"
    "drivers/ntt_vulkan/ntt_vulkan_shader_storage.cpp"
)

ntt_set_source_group("src/systems/render"
    NTT_APPLICATION_SOURCES
    "systems/render/render.h"
    "systems/render/render_driver.h"
    "systems/render/render_driver.cpp"
    "systems/render/render_system.h"
    "systems/render/render_system.cpp"
    "systems/render/mesh_view_storage.h"
    "systems/render/mesh_view_storage.cpp"
    "systems/render/mesh_storage.h"
    "systems/render/mesh_storage.cpp"
    "systems/render/texture_storage.h"
    "systems/render/texture_storage.cpp"
    "systems/render/shader_storage.h"
    "systems/render/shader_storage.cpp"
    "systems/render/render_globals.h"
    "systems/render/render_globals.cpp"
    "systems/render/uniform.h"
    "systems/render/uniform.cpp"
)

ntt_set_source_group("src/systems/render/defs"
    NTT_APPLICATION_SOURCES
    "systems/render/uniform_type.def"
    "systems/render/shader_input_topology.def"
    "resources/resource_type.def"
)

if (NTT_PLATFORM_WEB)
    ntt_embed_asset(
        "src/systems/render/shaders"
        NTT_APPLICATION_SOURCES
        "systems/render/shaders/gles/mesh.vs::generated_gles_mesh_vs.cpp::mesh_vs_data"
        "systems/render/shaders/gles/mesh.fs::generated_gles_mesh_fs.cpp::mesh_fs_data"
        "systems/render/shaders/gles/line.vs::generated_gles_line_vs.cpp::line_vs_data"
        "systems/render/shaders/gles/line.fs::generated_gles_line_fs.cpp::line_fs_data"
    )
else()
    ntt_embed_asset(
        "src/systems/render/shaders"
        NTT_APPLICATION_SOURCES
        "systems/render/shaders/opengl/mesh.vs::generated_opengl_mesh_vs.cpp::mesh_vs_data"
        "systems/render/shaders/opengl/mesh.fs::generated_opengl_mesh_fs.cpp::mesh_fs_data"
        "systems/render/shaders/opengl/line.vs::generated_opengl_line_vs.cpp::line_vs_data"
        "systems/render/shaders/opengl/line.fs::generated_opengl_line_fs.cpp::line_fs_data"
    )

    if (NTT_VULKAN)
        ntt_embed_asset(
            "src/systems/render/shaders"
            NTT_APPLICATION_SOURCES
            "systems/render/shaders/vulkan/mesh.vs::generated_vulkan_mesh_vs.cpp::vulkan_mesh_vs_data"
            "systems/render/shaders/vulkan/mesh.fs::generated_vulkan_mesh_fs.cpp::vulkan_mesh_fs_data"
            "systems/render/shaders/vulkan/line.vs::generated_vulkan_line_vs.cpp::vulkan_line_vs_data"
            "systems/render/shaders/vulkan/line.fs::generated_vulkan_line_fs.cpp::vulkan_line_fs_data"
        )
    endif()
endif()

ntt_set_source_group("src/resources"
    NTT_APPLICATION_SOURCES
    "resources/resource.h"
    "resources/resource.cpp"
    "resources/resource_type.h"
    "resources/resource_type.cpp"
    "resources/resources.h"
    "resources/texture_resource.h"
    "resources/texture_resource.cpp"
)

ntt_set_source_group("src/systems/render/components"
    NTT_APPLICATION_SOURCES
    "systems/render/components/components.h"
    "systems/render/components/mesh.h"
    "systems/render/components/mesh.cpp"
    "systems/render/components/vertex.h"
    "systems/render/components/vertex.cpp"
)

ntt_set_source_group("src/systems"
    NTT_APPLICATION_SOURCES
    "systems/systems.h"
    "systems/systems.cpp"
    "systems/system.h"
    "systems/system.cpp"
    "systems/system_globals.h"
    "systems/system_globals.cpp"
)

ntt_set_source_group("src/systems/display"
    NTT_APPLICATION_SOURCES
    "systems/display/display.h"
    "systems/display/display_system.h"
    "systems/display/display_system.cpp"
    "systems/display/display_driver.h"
    "systems/display/display_driver.cpp"
)

add_library(
    ${NTT_APPLICATION_LIBRARY_NAME}
    STATIC
    ${NTT_APPLICATION_SOURCES}
)

target_include_directories(
    ${NTT_APPLICATION_LIBRARY_NAME}
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

ntt_find_package(ntt-glfw REQUIRED "Dependencies")
ntt_find_package(ntt-stb REQUIRED "Dependencies")

target_link_libraries(
    ${NTT_APPLICATION_LIBRARY_NAME}
    PUBLIC
    ntt-services
    ntt-glfw
    ntt-stb
)

if (NOT NTT_PLATFORM_WEB)
    ntt_find_package(ntt-vulkan REQUIRED "Dependencies")

    target_link_libraries(
        ${NTT_APPLICATION_LIBRARY_NAME}
        PUBLIC
        ntt-vulkan
    )
endif()

ntt_add_options(${NTT_APPLICATION_LIBRARY_NAME})
ntt_add_link_flags(${NTT_APPLICATION_LIBRARY_NAME})
ntt_add_compile_options(${NTT_APPLICATION_LIBRARY_NAME})