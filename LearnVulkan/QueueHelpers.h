#pragma once

#include <vulkan/vulkan.hpp>

#include "QueueFamilyIndices.h"
#include "SwapChainSupportDetails.h"

#include <vector>
#include <set>

namespace QueueHelpers
{
	const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice device, const vk::SurfaceKHR surface);
	bool isDeviceSuitable(const vk::PhysicalDevice device, const vk::SurfaceKHR surface);
	bool checkDeviceExtensionSupport(const vk::PhysicalDevice device);
}