#pragma once

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif // !GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <functional>
#include <vector>
#include <algorithm>
#include <set>
#include <fstream>

#include "QueueFamilyIndices.h"
#include "QueueHelpers.h"
#include "SwapChainSupportDetails.h"

class HelloTriangleApplication
{
public:
	HelloTriangleApplication(const int width, const int height);

	void run();

private:
	int width, height;
	VkInstance instance;
	GLFWwindow* window;
	VkDebugReportCallbackEXT callback;
	VkSurfaceKHR surface;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device;
	VkQueue graphicsQueue;
	VkQueue presentQueue; // 'Present' as in Presentation
	VkSwapchainKHR swapchain;
	VkCommandPool commandPool;

	VkFormat swapchainImageFormat;
	VkExtent2D swapchainExtent;

	std::vector<VkImage> swapchainImages;
	std::vector<VkImageView> swapchainImageViews;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	std::vector<VkCommandBuffer> commandBuffers;

	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	static const std::vector<const char*> validationLayers;

	void initWindow();

	void initVulkan();

	void mainLoop();

	void drawFrame();

	void cleanup();

	void pickPhysicalDevice();

	void createLogicalDevice();

	bool checkValidationLayerSupport();

	std::vector<const char*> getRequiredExtensions();

	void createInstance();

	void setupDebugCallback();

	void createSurface();

	void createSwapChain();

	void createImageViews();

	void createRenderPass();

	void createGraphicsPipeline();

	void createFramebuffers();

	void createCommandPool();

	void createCommandBuffers();

	VkShaderModule createShaderModule(const std::vector<char>& code);

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugReportFlagsEXT flags,
		VkDebugReportObjectTypeEXT objType,
		uint64_t obj,
		size_t location,
		int32_t code,
		const char* layerPrefix,
		const char* msg,
		void* userData
	);

	static bool isDeviceSuitable(const VkPhysicalDevice& device);

	static std::vector<char> readFile(const std::string& filename);
};
