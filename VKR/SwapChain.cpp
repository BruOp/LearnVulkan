#include "SwapChain.h"


namespace vkr
{
	SwapChain::SwapChain()
	{
	}

	SwapChain::SwapChain(
		const vk::Device & device,
		const vk::PhysicalDevice & physicalDevice,
		const vk::SurfaceKHR & surface,
		const vkr::Window & window
	)
	{
		vkr::SwapChainSupportDetails swapChainSupport{ physicalDevice, surface };
		vkr::QueueFamilyChecker checker{ physicalDevice, surface };

		swapChainExtent = swapChainSupport.chooseSwapExtent(window);
		swapChainImageFormat = swapChainSupport.getSwapSurfaceFormat();
		// We're trying to create a swap chain that is at least large enough to support the swap present
		// mode we've chosen above, but not exceed the max.
		imageCount = swapChainSupport.capabilities.minImageCount + 1;
		const uint32_t maxImageCount = swapChainSupport.capabilities.maxImageCount;

		// A max of 0 == unlimited
		if (maxImageCount > 0 && imageCount > maxImageCount) {
			imageCount = maxImageCount;
		}

		auto surfaceFormat{ swapChainSupport.getSwapSurfaceFormat() };
		auto presentMode{ swapChainSupport.chooseSwapPresentMode() };

		auto swapchainImageFormat = surfaceFormat.format;

		vk::SwapchainCreateInfoKHR createInfo = {};
		createInfo.surface = surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = swapChainExtent;
		createInfo.imageArrayLayers = 1; // Always 1 unless you're creating stereoscopic
		createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment; // Direct (no post processing) rendering

		uint32_t queueFamilyIndices[] = { (uint32_t)checker.graphicsFamily, (uint32_t)checker.presentFamily };

		if (checker.graphicsFamily != checker.presentFamily) {
			createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		} else {
			createInfo.imageSharingMode = vk::SharingMode::eExclusive;
			createInfo.queueFamilyIndexCount = 0; // Optional
			createInfo.pQueueFamilyIndices = nullptr; // Optional
		}

		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		// What type of alpha blending we're going to use
		createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;

		createInfo.presentMode = presentMode;
		createInfo.setClipped(VK_TRUE);

		swapChain = device.createSwapchainKHR(createInfo);
	}

	SwapChain::SwapChain(SwapChain && otherSwapChain)
	{
		imageCount = otherSwapChain.imageCount;
		swapChainExtent = otherSwapChain.swapChainExtent;
		swapChainImageFormat = otherSwapChain.swapChainImageFormat;
		swapChain = std::move(otherSwapChain.swapChain);

		otherSwapChain.swapChain = vk::SwapchainKHR{};
	}

	SwapChain & SwapChain::operator=(SwapChain && otherSwapChain)
	{
		if (this != &otherSwapChain) {
			imageCount = otherSwapChain.imageCount;
			swapChainExtent = otherSwapChain.swapChainExtent;
			swapChainImageFormat = otherSwapChain.swapChainImageFormat;
			swapChain = std::move(otherSwapChain.swapChain);

			otherSwapChain.swapChain = vk::SwapchainKHR{};
		}
		return *this;
	}

	void SwapChain::destroy(const vk::Device & device)
	{
		device.destroySwapchainKHR(swapChain);
	}

	void SwapChain::getSwapChainImages(
		const vk::Device & device,
		std::vector<vk::Image>& swapChainImages
	)
	{
		device.getSwapchainImagesKHR(swapChain, &imageCount, nullptr);
		swapChainImages.resize(imageCount);
		device.getSwapchainImagesKHR(swapChain, &imageCount, swapChainImages.data());
	}
}
