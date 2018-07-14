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
			presentationModes.resize(presentModeCount);
			device.getSurfacePresentModesKHR(surface, &presentModeCount, presentationModes.data());
		}
	}

	bool SwapChainSupportDetails::isAdequate() const
	{
		return !formats.empty() && !presentationModes.empty();
	}
}