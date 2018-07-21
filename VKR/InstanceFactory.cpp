#include "InstanceFactory.h"


namespace vkr
{
	vk::Instance InstanceFactory::create(const std::vector<const char*> & validationLayers)
	{
		#ifdef NDEBUG
			if (!checkValidationLayerSupport()) {
				throw std::runtime_error("validation layers requested, but not available!");
			}
		#endif

		vk::ApplicationInfo appInfo = {};
		appInfo.pApplicationName = "Hello Triangle";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_1;

		vk::InstanceCreateInfo createInfo = {};
		createInfo.pApplicationInfo = &appInfo;

		auto extensions = getRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		#ifdef NDEBUG
			createInfo.enabledLayerCount = 0;
		#else
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		#endif

		auto instance = vk::createInstance(createInfo, nullptr);
		return instance;
	}

	std::vector<const char*> InstanceFactory::getRequiredExtensions()
	{
		std::vector<const char*> extensions;

		unsigned int glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		for (unsigned int i = 0; i < glfwExtensionCount; i++) {
			extensions.push_back(glfwExtensions[i]);
		}

		#ifndef NDEBUG
			extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
		#endif

		return extensions;
	}

	bool InstanceFactory::checkValidationLayerSupport(const std::vector<const char*> & validationLayers)
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : validationLayers) {
			auto& match = std::find_if(availableLayers.begin(), availableLayers.end(),
				[layerName](const VkLayerProperties& layer) -> bool {
				return std::strcmp(layer.layerName, layerName) == 0;
			}
			);

			if (match == availableLayers.end()) {
				return false;
			}
		}

		return true;
	}

}
