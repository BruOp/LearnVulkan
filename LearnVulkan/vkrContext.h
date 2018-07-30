#pragma once

#include <vulkan\vulkan.hpp>
#include <vma/vk_mem_alloc.h>

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
// VKR
#include "Buffer.h"
#include "BufferUtils.h"
#include "Image.h"
#include "ImageView.h"
#include "Texture.h"
#include "InstanceFactory.h"
#include "QueueFamilyChecker.h"
#include "GraphicsPipelineFactory.h"
#include "LogicalDeviceFactory.h"
#include "Window.h"
#include "SwapChain.h"
#include "CommandManager.h"

class vkrContext
{
public:
	vkrContext(const int width, const int height);

	void run();

private:
	int width, height;

	VmaAllocator vulkanAllocator;
	vk::Instance instance;
	vkr::Window window;
	VkDebugReportCallbackEXT callback; // Keeping this vanilla to spare myself some headaches
	vk::SurfaceKHR surface;
	vk::PhysicalDevice physicalDevice;
	vk::Device device;
	vkr::SwapChain swapchain;

    vkr::QueueFamilyChecker queueFamilyChecker;
    vkr::CommandManager commandManager;

	std::vector<vk::Image> swapchainImages;
	std::vector<vkr::ImageView> swapchainImageViews;
	std::vector<vk::Framebuffer> swapChainFramebuffers;
	std::vector<vk::CommandBuffer> commandBuffers;

	vk::RenderPass renderPass;
	vk::DescriptorSetLayout descriptorSetLayout;
	vk::UniquePipelineLayout pipelineLayout;
	vk::UniquePipeline graphicsPipeline;

	vk::Semaphore imageAvailableSemaphore;
	vk::Semaphore renderFinishedSemaphore;

	vkr::Buffer vertexBuffer;
	vkr::Buffer indexBuffer;

    vkr::Texture texture;

	std::vector<vkr::Buffer> uniformBuffers;

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

    void initCommandManager();

	void createAllocator();

	void setupDebugCallback();

	void createSwapChain();

	void createImageViews();

	void createRenderPass();

	void createDescriptorSetLayout();

	void createGraphicsPipeline();

	void createFramebuffers();

    void createTextureImage();

	void createVertexBuffer();

	void createIndexBuffer();

	void createUniformBuffers();

	void createDescriptorPool();

	void createDescriptorSets();

	void createCommandBuffers();

	void createSemaphores();

	void recreateSwapChain();

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
