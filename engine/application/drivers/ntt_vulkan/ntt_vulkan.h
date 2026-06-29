#if NTT_VULKAN

#pragma once
#include "systems/render/render_driver.h"

namespace ntt {

Result RegisterVulkanDriver();

Result RegisterVulkanRenderer();

} // namespace ntt

#endif // NTT_VULKAN