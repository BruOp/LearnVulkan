#pragma once

#include <vulkan\vulkan.hpp>

#include <iostream>
#include <stdexcept>
#include <functional>
#include <vector>
#include <algorithm>
#include <set>
#include <fstream>
#include <chrono>

#include "SwapChainSupportDetails.h"
#include "Vertex.h"
#include "TransformationBufferObject.h"

#include "QueueFamilyChecker.h"
#include "GraphicsPipelineFactory.h"
#include "LogicalDeviceFactory.h"
#include "Window.h"
#include "SwapChainFactory.h"

class HelloTriangleApplication
{
public:
	HelloTriangleApplication(const int width, const int height);

	void run();

private:
	int width, height;

	vk::Instance instance;
	vkr::Window window;
	VkDebugReportCallbackEXT callback; // Keeping this vanilla to spare myself some headaches
	vk::SurfaceKHR surface;
	vk::PhysicalDevice physicalDevice;
	vk::Device device;
	vk::Queue graphicsQueue;
	vk::Queue presentQueue; // 'Present' as in Presentation
	vkr::SwapChain swapchain;
	vk::CommandPool commandPool;

	std::vector<vk::Image> swapchainImages;
	std::vector<vk::ImageView> swapchainImageViews;
	std::vector<vk::Framebuffer> swapChainFramebuffers;
	std::vector<vk::CommandBuffer> commandBuffers;

	vk::RenderPass renderPass;
	vk::DescriptorSetLayout descriptorSetLayout;
	vk::UniquePipelineLayout pipelineLayout;
	vk::UniquePipeline graphicsPipeline;

	vk::Semaphore imageAvailableSemaphore;
	vk::Semaphore renderFinishedSemaphore;

	vk::Buffer vertexBuffer;
	vk::DeviceMemory vertexBufferMemory;
	vk::Buffer indexBuffer;
	vk::DeviceMemory indexBufferMemory;

	std::vector<vk::Buffer> uniformBuffers;
	std::vector<vk::DeviceMemory> uniformBuffersMemory;

	vk::DescriptorPool descriptorPool;
	std::vector<vk::DescriptorSet> descriptorSets;

	static const std::vector<const char*> validationLayers;

	void initWindow();

	void initVulkan();

	void mainLoop();

	void drawFrame();

	void cleanup();

	void cleanupSwapChain();

	void createInstance();

	void pickPhysicalDevice();

	void createSurface();

	void createLogicalDevice();

	bool checkValidationLayerSupport();

	std::vector<const char*> getRequiredExtensions();

	void setupDebugCallback();

	void createSwapChain();

	void createImageViews();

	void createRenderPass();

	void createDescriptorSetLayout();

	void createGraphicsPipeline();

	void createFramebuffers();

	void createCommandPool();

	void createVertexBuffer();

	void createIndexBuffer();

	void createUniformBuffers();

	void createDescriptorPool();

	void createDescriptorSets();

	void createCommandBuffers();

	void createSemaphores();

	void recreateSwapChain();

	void createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& bufferMemory);
	void copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);

	void updateUniformBuffer(uint32_t currentImage);

	vk::ShaderModule createShaderModule(const std::vector<char>& code);

	uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);

	//static vk::DebugReportCallbackCreateInfoEXT::pfnCallback debugCallback()
	static vk::Bool32 debugCallback(
		vk::DebugReportFlagsEXT flags,
		vk::DebugReportObjectTypeEXT objType,
		uint64_t obj,
		size_t location,
		int32_t code,
		const char* layerPrefix,
		const char* msg,
		void* userData
	);

	static bool isDeviceSuitable(const vk::PhysicalDevice& device);

	static std::vector<char> readFile(const std::string& filename);

	static void onWindowResize(GLFWwindow * window, int width, int height);
};
