#pragma once

#include<vulkan/vulkan.hpp>
#include <vector>

namespace vkr
{
	struct SwapChainSupportDetails
	{
		SwapChainSupportDetails(const vk::PhysicalDevice device, const vk::SurfaceKHR surface);

		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> presentationModes;

		bool isAdequate() const;
	};
}