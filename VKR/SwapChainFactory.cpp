#include "SwapChainFactory.h"


namespace vkr
{

	SwapChainFactory::SwapChainFactory(
		const vk::PhysicalDevice & physicalDevice,
		const vk::SurfaceKHR & surface
	) :
		checker(physicalDevice, surface),
		swapChainSupport(physicalDevice, surface)
	{
		// We're trying to create a swap chain that is at least large enough to support the swap present
		// mode we've chosen above, but not exceed the max.
		imageCount = swapChainSupport.capabilities.minImageCount + 1;
		const uint32_t maxImageCount = swapChainSupport.capabilities.maxImageCount;

		// A max of 0 == unlimited
		if (maxImageCount > 0 && imageCount > maxImageCount) {
			imageCount = maxImageCount;
		}
	}

	vk::SwapchainKHR SwapChainFactory::create(
		const vk::Device & device,
		const vk::Extent2D & swapChainExtent,
		const vk::SurfaceKHR & surface
	)
	{
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

		vk::SwapchainKHR swapchain = device.createSwapchainKHR(createInfo);
		return swapchain;
	}

	void SwapChainFactory::getSwapChainImages(
		const vk::Device & device,
		const vk::SwapchainKHR swapchain,
		std::vector<vk::Image>& swapChainImages
	)
	{
		device.getSwapchainImagesKHR(swapchain, &imageCount, nullptr);
		swapChainImages.resize(imageCount);
		device.getSwapchainImagesKHR(swapchain, &imageCount, swapChainImages.data());
	}
	vk::Extent2D SwapChainFactory::getSwapChainExtent(const vkr::Window & window) const
	{
		return swapChainSupport.chooseSwapExtent(window);
	}
}
