#include "LogicalDeviceFactory.h"


namespace vkr
{
	LogicalDeviceFactory::LogicalDeviceFactory(const vk::PhysicalDevice physicalDevice, const vk::SurfaceKHR surface) :
		queueFamilyChecker(physicalDevice, surface)
	{
	}

	vk::Device LogicalDeviceFactory::create(
		const vk::PhysicalDevice physicalDevice,
		const vk::SurfaceKHR surface
	)
	{
		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
		std::set<int> uniqueQueueFamilies = { queueFamilyChecker.graphicsFamily, queueFamilyChecker.presentFamily };

		float queuePriority = 1.0f;
		for (const int queueFamily : uniqueQueueFamilies) {
			vk::DeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		vk::PhysicalDeviceFeatures deviceFeatures = {};
        deviceFeatures.samplerAnisotropy = VK_TRUE;

		vk::DeviceCreateInfo createInfo = {};
		createInfo.queueCreateInfoCount = static_cast<int>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.pEnabledFeatures = &deviceFeatures;

		createInfo.enabledExtensionCount = static_cast<uint32_t>(QueueFamilyChecker::requiredExtensions.size());
		createInfo.ppEnabledExtensionNames = QueueFamilyChecker::requiredExtensions.data();

	#ifdef NDEBUG
		createInfo.enabledLayerCount = 0;
	#else
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	#endif

		vk::Device device { physicalDevice.createDevice(createInfo) };
		return device;
	}

	vk::Queue LogicalDeviceFactory::getGraphicsQueue(const vk::Device device)
	{
		vk::Queue queue{ device.getQueue(queueFamilyChecker.graphicsFamily, 0)};
		return queue;
	}

	vk::Queue LogicalDeviceFactory::getPresentQueue(const vk::Device device)
	{
		vk::Queue queue{ device.getQueue(queueFamilyChecker.presentFamily, 0) };
		return queue;
	}

	const std::vector<const char*> LogicalDeviceFactory::validationLayers;
}