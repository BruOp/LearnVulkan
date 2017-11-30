#pragma once

#include "QueueFamilyIndices.h"

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif // !GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <vector>

namespace QueueHelpers
{
	QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice device, const VkSurfaceKHR surface);
	bool isDeviceSuitable(const VkPhysicalDevice device, const VkSurfaceKHR surface);
}