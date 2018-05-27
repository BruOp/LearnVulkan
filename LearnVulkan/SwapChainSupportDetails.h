#pragma once

#include<vulkan/vulkan.hpp>
#include <vector>

struct SwapChainSupportDetails
{
	vk::SurfaceCapabilitiesKHR capabilities;
	std::vector<vk::SurfaceFormatKHR> formats;
	std::vector<vk::PresentModeKHR> presentationModes;

	bool adequate() const;

	static SwapChainSupportDetails querySwapChainSupport(const vk::PhysicalDevice device, const vk::SurfaceKHR surface);
};