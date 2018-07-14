#pragma once

#include <vulkan/vulkan.hpp>
#include <set>

#include "SwapChainSupportDetails.h"

namespace vkr
{
	class QueueFamilyChecker
	{
	public:
		QueueFamilyChecker(const vk::PhysicalDevice & phyDevice, const vk::SurfaceKHR & surface);

		int graphicsFamily;
		int presentFamily;
		bool isSuitable;

		inline bool isComplete() { return graphicsFamily >= 0 && presentFamily >= 0; };

		static const std::vector<const char*> requiredExtensions;

	private:

		bool isDeviceSuitable(const vk::PhysicalDevice & phyDevice, const vk::SurfaceKHR & surface);
		bool checkDeviceExtensionSupport(const vk::PhysicalDevice & phyDevice, const vk::SurfaceKHR & surface);

	};
}

