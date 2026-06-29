#if NTT_VULKAN

#pragma once

#include "vulkan/vulkan.h"

#if NTT_GLFW
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#endif // NTT_GLFW

#define _VK_ASSERT(exp, ret)                                                                                           \
	do                                                                                                                 \
	{                                                                                                                  \
		VkResult result = (exp);                                                                                       \
		if (result != VK_SUCCESS)                                                                                      \
		{                                                                                                              \
			NTT_VULKAN_ERROR("Vulkan error: %d", result);                                                              \
			return ret;                                                                                                \
		}                                                                                                              \
	} while (0)

#define VK_ASSERT_B(exp) _VK_ASSERT(exp, false)

#define VK_ASSERT(exp) _VK_ASSERT(exp, RESULT_VULKAN_ERROR)

#endif // NTT_VULKAN