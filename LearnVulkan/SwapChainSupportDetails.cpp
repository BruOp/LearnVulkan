#include "SwapChainSupportDetails.h"

bool SwapChainSupportDetails::adequate() const
{
	return !formats.empty() && !presentationModes.empty();
}

SwapChainSupportDetails SwapChainSupportDetails::querySwapChainSupport(
	const vk::PhysicalDevice device,
	const vk::SurfaceKHR surface)
{
	SwapChainSupportDetails details;

	device.getSurfaceCapabilitiesKHR(surface, &details.capabilities);

	uint32_t formatCount;
	device.getSurfaceFormatsKHR(surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		device.getSurfaceFormatsKHR(surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	device.getSurfacePresentModesKHR(surface, &presentModeCount, nullptr);

	if (presentModeCount > 0) {
		details.presentationModes.resize(presentModeCount);
		device.getSurfacePresentModesKHR(surface, &presentModeCount, details.presentationModes.data());
	}

	return details;
}