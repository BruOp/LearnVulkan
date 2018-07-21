#pragma once

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

namespace vkr {
	namespace InstanceFactory {
		vk::Instance create(const std::vector<const char*> & validationLayers);

		std::vector<const char*> getRequiredExtensions();
		bool checkValidationLayerSupport(const std::vector<const char*> & validationLayers);
	}
}

