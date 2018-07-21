#pragma once

#include <vulkan/vulkan.hpp>
#include "SwapChainSupportDetails.h"
#include "QueueFamilyChecker.h"

namespace vkr
{

	class SwapChainFactory
	{
	public:
		SwapChainFactory(
			const vk::PhysicalDevice & physicalDevice,
			const vk::SurfaceKHR & surface
		);

		vk::SwapchainKHR create(
			const vk::Device & device,
			const vk::Extent2D & swapChainExtent,
			const vk::SurfaceKHR & surface
		);

		void getSwapChainImages(
			const vk::Device & device,
			vk::SwapchainKHR swapchain,
			std::vector<vk::Image> & swapChainImages
		);

		vk::Extent2D getSwapChainExtent(const vkr::Window& window) const;
	private:
		vkr::SwapChainSupportDetails swapChainSupport;
		vkr::QueueFamilyChecker checker;
		uint32_t imageCount;
	};
}

