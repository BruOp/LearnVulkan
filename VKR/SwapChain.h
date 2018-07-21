#pragma once

#include <vulkan/vulkan.hpp>
#include "SwapChainSupportDetails.h"
#include "QueueFamilyChecker.h"

namespace vkr
{

	class SwapChain
	{
	public:
		SwapChain();
		SwapChain(
			const vk::Device & device,
			const vk::PhysicalDevice & physicalDevice,
			const vk::SurfaceKHR & surface,
			const vkr::Window & window
		);

		SwapChain(const SwapChain&) = delete;
		SwapChain& operator=(const SwapChain&) = delete;

		SwapChain(SwapChain&& swapChain);
		SwapChain& operator=(SwapChain&& swapChain);

		void destroy(const vk::Device & device);

		uint32_t imageCount;
		vk::Extent2D swapChainExtent;
		vk::SurfaceFormatKHR swapChainImageFormat;

		inline operator vk::SwapchainKHR() const { return swapChain; };

		void getSwapChainImages(
			const vk::Device & device,
			std::vector<vk::Image> & swapChainImages
		);

	private:
		vk::SwapchainKHR swapChain;

	};
}

