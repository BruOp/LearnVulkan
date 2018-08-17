#include "SwapChainSupportDetails.h"

namespace vkr
{
	SwapChainSupportDetails::SwapChainSupportDetails(const vk::PhysicalDevice device, const vk::SurfaceKHR surface)
	{
		device.getSurfaceCapabilitiesKHR(surface, &capabilities);

		uint32_t formatCount;
		device.getSurfaceFormatsKHR(surface, &formatCount, nullptr);

		if (formatCount != 0) {
			formats.resize(formatCount);
			device.getSurfaceFormatsKHR(surface, &formatCount, formats.data());
		}

		uint32_t presentModeCount;
		device.getSurfacePresentModesKHR(surface, &presentModeCount, nullptr);

		if (presentModeCount > 0) {
			presentModes.resize(presentModeCount);
			device.getSurfacePresentModesKHR(surface, &presentModeCount, presentModes.data());
		}
	}

	bool SwapChainSupportDetails::isAdequate() const
	{
		return !formats.empty() && !presentModes.empty();
	}

	vk::SurfaceFormatKHR SwapChainSupportDetails::getSwapSurfaceFormat()
	{
		if (formats.size() == 1 && formats[0].format == vk::Format::eUndefined) {
			return { vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear };
		}

		for (const auto& availableFormat : formats) {
			if (availableFormat.format == vk::Format::eB8G8R8A8Unorm && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
				return availableFormat;
			}
		}

		// If the above two tests fail then we'll just return the first available format
		return formats[0];
	}

	vk::PresentModeKHR SwapChainSupportDetails::chooseSwapPresentMode()
	{
		vk::PresentModeKHR bestMode = vk::PresentModeKHR::eFifo;
		for (const auto& presentMode : presentModes) {
			if (presentMode == vk::PresentModeKHR::eMailbox) {
				return presentMode;
			} else if (presentMode == vk::PresentModeKHR::eImmediate) {
				bestMode = presentMode;
			}
		}

		return bestMode;
	}

	vk::Extent2D SwapChainSupportDetails::chooseSwapExtent(const vkr::Window & window) const
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		} else {
			vk::Extent2D actualExtent = window.getExtent();

			actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

			return actualExtent;
		}
	}
}