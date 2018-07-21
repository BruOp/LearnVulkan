#pragma once

#include<vulkan/vulkan.hpp>
#include <vector>
#include "Window.h"

namespace vkr
{
	struct SwapChainSupportDetails
	{
		SwapChainSupportDetails(const vk::PhysicalDevice device, const vk::SurfaceKHR surface);

		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> presentModes;

		bool isAdequate() const;
		vk::SurfaceFormatKHR getSwapSurfaceFormat();
		vk::PresentModeKHR chooseSwapPresentMode();
		vk::Extent2D chooseSwapExtent(const vkr::Window & window) const;
	};
}