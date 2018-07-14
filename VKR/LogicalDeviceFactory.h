#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>

#include "QueueFamilyChecker.h"

namespace vkr
{
	class LogicalDeviceFactory
	{
	public:
		LogicalDeviceFactory(const vk::PhysicalDevice physicalDevice, const vk::SurfaceKHR surface);

		vk::Device create(const vk::PhysicalDevice physicalDevice, const vk::SurfaceKHR surface);

		vk::Queue getGraphicsQueue(const vk::Device device);
		vk::Queue getPresentQueue(const vk::Device device);

		static const std::vector<const char*> validationLayers;
	private:

		QueueFamilyChecker queueFamilyChecker;

	};
}
