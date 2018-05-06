#pragma once

#include "QueueFamilyIndices.h"
#include "SwapChainSupportDetails.h"

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif // !GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <vector>
#include <set>

namespace QueueHelpers
{
	const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice device, const VkSurfaceKHR surface);
	bool isDeviceSuitable(const VkPhysicalDevice device, const VkSurfaceKHR surface);
	bool checkDeviceExtensionSupport(const VkPhysicalDevice device);
}