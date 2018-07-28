#include "QueueFamilyChecker.h"


namespace vkr
{
    QueueFamilyChecker::QueueFamilyChecker() :
        graphicsFamily(-1),
        presentFamily(-1),
        isSuitable(false)
    {
    }

    QueueFamilyChecker::QueueFamilyChecker(
		const vk::PhysicalDevice & device,
		const vk::SurfaceKHR & surface
	) :
		graphicsFamily(-1),
		presentFamily(-1),
		isSuitable(false)
	{
		uint32_t queueFamilyCount = 0;
		device.getQueueFamilyProperties(&queueFamilyCount, nullptr);

		std::vector<vk::QueueFamilyProperties> queueFamilies(queueFamilyCount);
		device.getQueueFamilyProperties(&queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
				graphicsFamily = i;
			}

			vk::Bool32 presentSupport = false;
			device.getSurfaceSupportKHR(i, surface, &presentSupport);
			if (queueFamily.queueCount > 0 && presentSupport) {
				presentFamily = i;
			}

			if (isComplete()) {
				break;
			}

			++i;
		}

		isSuitable = isDeviceSuitable(device, surface);
	}

	bool QueueFamilyChecker::isDeviceSuitable(const vk::PhysicalDevice & phyDevice, const vk::SurfaceKHR & surface)
	{
		bool extensionsSupported = checkDeviceExtensionSupport(phyDevice, surface);

		bool swapChainAdequate = false;
		if (extensionsSupported) {
			SwapChainSupportDetails swapChainSupport{ phyDevice, surface };
			swapChainAdequate = swapChainSupport.isAdequate();
		};
		return isComplete() && extensionsSupported && swapChainAdequate;
	}

	bool QueueFamilyChecker::checkDeviceExtensionSupport(const vk::PhysicalDevice & device, const vk::SurfaceKHR & surface)
	{
		uint32_t extensionCount;
		device.enumerateDeviceExtensionProperties(nullptr, &extensionCount, nullptr);

		std::vector<vk::ExtensionProperties> availableExtensions(extensionCount);
		device.enumerateDeviceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(requiredExtensions.begin(), requiredExtensions.end());

		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	const std::vector<const char*> QueueFamilyChecker::requiredExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
}
