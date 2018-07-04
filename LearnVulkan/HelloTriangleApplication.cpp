#include "HelloTriangleApplication.h"

const std::vector<const char*> HelloTriangleApplication::validationLayers = {
	"VK_LAYER_LUNARG_standard_validation"
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

VkResult CreateDebugReportCallbackEXT(vk::Instance instance, const VkDebugReportCallbackCreateInfoEXT * pCreateInfo, const VkAllocationCallbacks * pAllocator, VkDebugReportCallbackEXT * pCallback)
{
	auto func = (PFN_vkCreateDebugReportCallbackEXT)instance.getProcAddr("vkCreateDebugReportCallbackEXT");
	if (func != nullptr) {
		return func((VkInstance)instance, pCreateInfo, pAllocator, pCallback);
	} else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugReportCallbackEXT(vk::Instance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugReportCallbackEXT)instance.getProcAddr("vkDestroyDebugReportCallbackEXT");
	if (func != nullptr) {
		func((VkInstance)instance, callback, pAllocator);
	}
}

const std::vector<Vertex> vertices = {
	{ { -0.5f, -0.5f, 0.5f }, { 1.0f, 0.0f, 0.0f } },
	{ {  0.5f, -0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f } },
	{ {  0.5f,  0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } },
	{ { -0.5f,  0.5f, 0.5f }, { 1.0f, 1.0f, 1.0f } }
};

const std::vector<uint16_t> indices = {
	0, 1, 2, 2, 3, 0
};

HelloTriangleApplication::HelloTriangleApplication(int width, int height) :
	width(width),
	height(height)
{
}

void HelloTriangleApplication::run()
{
	initWindow();
	initVulkan();
	mainLoop();
	cleanup();
}

void HelloTriangleApplication::initWindow()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	window = glfwCreateWindow(width, height, "Vulkan", nullptr, nullptr);

	glfwSetWindowUserPointer(window, this);
	glfwSetWindowSizeCallback(window, HelloTriangleApplication::onWindowResize);
}

void HelloTriangleApplication::initVulkan()
{
	createInstance();
	setupDebugCallback();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
	createImageViews();
	createRenderPass();
	createDescriptorSetLayout();
	createGraphicsPipeline();
	createFramebuffers();
	createCommandPool();
	createVertexBuffer();
	createIndexBuffer();
	createUniformBuffers();
	createDescriptorPool();
	createDescriptorSets();
	createCommandBuffers();
	createSemaphores();
}

void HelloTriangleApplication::mainLoop()
{
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		drawFrame();
	}
	device.waitIdle();
}

void HelloTriangleApplication::drawFrame()
{
	//  Acquire an image from the swap chain
	//	Execute the command buffer with that image as attachment in the framebuffer
	//	Return the image to the swap chain for presentation
	uint32_t imageIndex;
	presentQueue.waitIdle();
	
	vk::Result result = device.acquireNextImageKHR(
		swapchain,
		std::numeric_limits<uint32_t>::max(),
		imageAvailableSemaphore,
		vk::Fence{},
		&imageIndex
	);
	
	if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR) {
		recreateSwapChain();
		return;
	} else if (result != vk::Result::eSuccess) {
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	updateUniformBuffer(imageIndex);

	vk::SubmitInfo submitInfo = {};
	
	vk::Semaphore waitSemaphores[] = { imageAvailableSemaphore };
	vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

	vk::Semaphore signalSemaphores[] = { renderFinishedSemaphore };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	graphicsQueue.submit(1, &submitInfo, vk::Fence{});

	vk::PresentInfoKHR presentInfo = {};
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	vk::SwapchainKHR swapChains[] = { swapchain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;
	result = presentQueue.presentKHR(&presentInfo);

	if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR) {
		recreateSwapChain();
	} else if (result != vk::Result::eSuccess) {
		throw std::runtime_error("failed to present swap chain image!");
	}
}

void HelloTriangleApplication::cleanup()
{
	cleanupSwapChain();

	device.destroyDescriptorPool(descriptorPool);
	device.destroyDescriptorSetLayout(descriptorSetLayout);

	for (size_t i = 0; i < swapchainImages.size(); i++) {
		device.destroyBuffer(uniformBuffers[i]);
		device.freeMemory(uniformBuffersMemory[i]);
	}

	device.destroyBuffer(indexBuffer);
	device.freeMemory(indexBufferMemory);

	device.destroyBuffer(vertexBuffer);
	device.freeMemory(vertexBufferMemory);

	device.destroySemaphore(renderFinishedSemaphore);
	device.destroySemaphore(imageAvailableSemaphore);

	device.destroyCommandPool(commandPool);

	device.destroy();
	DestroyDebugReportCallbackEXT(instance, callback, nullptr);
	instance.destroySurfaceKHR(surface);
	instance.destroy();
	glfwDestroyWindow(window);

	glfwTerminate();
}

void HelloTriangleApplication::cleanupSwapChain()
{
	for (auto & framebuffer : swapChainFramebuffers) {
		device.destroyFramebuffer(framebuffer);
	}

	device.freeCommandBuffers(commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
	
	device.destroyPipeline(graphicsPipeline);
	device.destroyPipelineLayout(pipelineLayout);
	device.destroyRenderPass(renderPass);

	for (auto imageView : swapchainImageViews) {
		device.destroyImageView(imageView);
	}

	device.destroySwapchainKHR(swapchain);
}

void HelloTriangleApplication::createInstance()
{
	if (enableValidationLayers && !checkValidationLayerSupport()) {
		throw std::runtime_error("validation layers requested, but not available!");
	}

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

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	} else {
		createInfo.enabledLayerCount = 0;
	}
	instance = vk::createInstance(createInfo, nullptr);
}

void HelloTriangleApplication::pickPhysicalDevice()
{
	uint32_t deviceCount = 0;
	instance.enumeratePhysicalDevices(&deviceCount, nullptr);

	if (deviceCount == 0) {
		throw std::runtime_error("Faily to find GPUs with Vulkan support!");
	}

	std::vector<vk::PhysicalDevice> devices(deviceCount);
	instance.enumeratePhysicalDevices(&deviceCount, devices.data());

	auto& deviceIter = std::find_if(devices.begin(), devices.end(), isDeviceSuitable);
	if (deviceIter == devices.end()) {
		throw std::runtime_error("Failed to find a suitable GPU!");
	} else {
		physicalDevice = *deviceIter;
	}
}

void HelloTriangleApplication::createSurface()
{
	auto vanillaVkSurface = VkSurfaceKHR(surface);
	if (glfwCreateWindowSurface(static_cast<VkInstance>(instance), window, nullptr, &vanillaVkSurface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create a window surface!");
	}
	surface = vk::SurfaceKHR{ vanillaVkSurface };
}

void HelloTriangleApplication::createLogicalDevice()
{
	QueueFamilyIndices indices = QueueHelpers::findQueueFamilies(physicalDevice, surface);

	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
	std::set<int> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily };

	float queuePriority = 1.0f;
	for (const int queueFamily : uniqueQueueFamilies) {
		vk::DeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	vk::PhysicalDeviceFeatures deviceFeatures = {};
	vk:: DeviceCreateInfo createInfo = {};
	createInfo.queueCreateInfoCount = static_cast<int>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = static_cast<uint32_t>(QueueHelpers::deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = QueueHelpers::deviceExtensions.data();

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	} else {
		createInfo.enabledLayerCount = 0;
	}

	physicalDevice.createDevice(&createInfo, nullptr, &device);
	device.getQueue(indices.graphicsFamily, 0, &graphicsQueue);
	device.getQueue(indices.presentFamily, 0, &presentQueue);
}

void HelloTriangleApplication::setupDebugCallback()
{
	if (!enableValidationLayers) return;

	vk::DebugReportCallbackCreateInfoEXT createDebugInfo = {};
	createDebugInfo.flags = vk::DebugReportFlagBitsEXT::eError | vk::DebugReportFlagBitsEXT::eWarning;
	createDebugInfo.pfnCallback = (PFN_vkDebugReportCallbackEXT)debugCallback;

	VkResult res = CreateDebugReportCallbackEXT(instance, &(VkDebugReportCallbackCreateInfoEXT)createDebugInfo, nullptr, &callback);
	if (res != VK_SUCCESS) {
		throw std::runtime_error("Failed to set up debug callback");
	}
}

std::vector<const char*> HelloTriangleApplication::getRequiredExtensions()
{
	std::vector<const char*> extensions;

	unsigned int glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	for (unsigned int i = 0; i < glfwExtensionCount; i++) {
		extensions.push_back(glfwExtensions[i]);
	}

	if (enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	}

	return extensions;
}

bool HelloTriangleApplication::checkValidationLayerSupport()
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

void HelloTriangleApplication::createSwapChain()
{
	SwapChainSupportDetails swapChainSupport = SwapChainSupportDetails::querySwapChainSupport(physicalDevice, surface);

	vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentationModes);
	swapchainExtent = chooseSwapExtent(swapChainSupport.capabilities);

	swapchainImageFormat = surfaceFormat.format;

	// We're trying to create a swap chain that is at least large enough to support the swap present
	// mode we've chosen above, but not exceed the max.
	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	const uint32_t maxImageCount = swapChainSupport.capabilities.maxImageCount;

	// A max of 0 == unlimited
	if (maxImageCount > 0 && imageCount > maxImageCount) {
		imageCount = maxImageCount;
	}

	vk::SwapchainCreateInfoKHR createInfo = {};
	createInfo.surface = surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = swapchainExtent;
	createInfo.imageArrayLayers = 1; // Always 1 unless you're creating stereoscopic
	createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment; // Direct (no post processing) rendering

	QueueFamilyIndices indices = QueueHelpers::findQueueFamilies(physicalDevice, surface);

	uint32_t queueFamilyIndices[] = { (uint32_t)indices.graphicsFamily, (uint32_t)indices.presentFamily };

	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	} else {
		createInfo.imageSharingMode = vk::SharingMode::eExclusive;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	// What type of alpha blending we're going to use
	createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;

	createInfo.presentMode = presentMode;
	createInfo.setClipped(VK_TRUE);
	
	device.createSwapchainKHR(&createInfo, nullptr, &swapchain);

	device.getSwapchainImagesKHR(swapchain, &imageCount, nullptr);
	swapchainImages.resize(imageCount);
	device.getSwapchainImagesKHR(swapchain, &imageCount, swapchainImages.data());
}

void HelloTriangleApplication::createImageViews()
{
	swapchainImageViews.resize(swapchainImages.size());

	for (size_t i = 0; i < swapchainImages.size(); i++) {
		vk::ImageViewCreateInfo createInfo = {};
		createInfo.image = swapchainImages[i];

		createInfo.viewType = vk::ImageViewType::e2D;
		createInfo.format = swapchainImageFormat;
		// Default mapping for each channel;
		createInfo.components.r = vk::ComponentSwizzle::eIdentity;
		createInfo.components.g = vk::ComponentSwizzle::eIdentity;
		createInfo.components.b = vk::ComponentSwizzle::eIdentity;
		createInfo.components.a = vk::ComponentSwizzle::eIdentity;
		// If we were working on a steroscopic 3D application then we might need multiple layers
		createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		device.createImageView(&createInfo, nullptr, &swapchainImageViews[i]);
	}
}

void HelloTriangleApplication::createRenderPass()
{
	vk::AttachmentDescription colorAttachment = {};
	colorAttachment.format = swapchainImageFormat;
	colorAttachment.samples = vk::SampleCountFlagBits::e1;
	colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
	colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
	colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
	colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
	colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
	colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

	vk::AttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

	vk::SubpassDescription subpass = {};
	subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	vk::RenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	vk::SubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;

	dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	dependency.srcAccessMask = (vk::AccessFlagBits)0;

	dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;

	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	device.createRenderPass(&renderPassInfo, nullptr, &renderPass);
}

void HelloTriangleApplication::createDescriptorSetLayout()
{
	vk::DescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;
	
	vk::DescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &uboLayoutBinding;

	descriptorSetLayout = device.createDescriptorSetLayout(layoutInfo);
}

void HelloTriangleApplication::createGraphicsPipeline()
{
	auto vertShaderCode = readFile("shaders/vert.spv");
	auto fragShaderCode = readFile("shaders/frag.spv");

	vk::ShaderModule vertShaderModule = createShaderModule(vertShaderCode);
	vk::ShaderModule fragShaderModule = createShaderModule(fragShaderCode);

	vk::PipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";
	// Note to self: keep pSpecializationInfo in mind when working with Vulkan shaders

	vk::PipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.stage = vk::ShaderStageFlagBits::eFragment;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	vk::PipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescriptions = Vertex::getAttributeDescriptions();

	vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	vk::PipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	vk::Viewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)swapchainExtent.width;
	viewport.height = (float)swapchainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	vk::Rect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = swapchainExtent;

	vk::PipelineViewportStateCreateInfo viewportState = {};
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	vk::PipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = vk::PolygonMode::eFill;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = vk::CullModeFlagBits::eBack;
	rasterizer.frontFace = vk::FrontFace::eCounterClockwise;
	rasterizer.depthBiasEnable = VK_FALSE;

	vk::PipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;

	vk::PipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
	colorBlendAttachment.blendEnable = VK_FALSE;

	vk::PipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = vk::LogicOp::eCopy;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 0;

	device.createPipelineLayout(&pipelineLayoutInfo, nullptr, &pipelineLayout);

	vk::GraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;

	device.createGraphicsPipelines(vk::PipelineCache{}, 1, &pipelineInfo, nullptr, &graphicsPipeline);
	device.destroyShaderModule(fragShaderModule, nullptr);
	device.destroyShaderModule(vertShaderModule, nullptr);
}

void HelloTriangleApplication::createFramebuffers()
{
	swapChainFramebuffers.resize(swapchainImageViews.size());

	for (size_t i = 0; i < swapchainImageViews.size(); i++) {
		vk::ImageView attachments[] = {
			swapchainImageViews[i]
		};

		vk::FramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = swapchainExtent.width;
		framebufferInfo.height = swapchainExtent.height;
		framebufferInfo.layers = 1;
		
		device.createFramebuffer(&framebufferInfo, nullptr, &swapChainFramebuffers[i]);
	}
}

void HelloTriangleApplication::createCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = QueueHelpers::findQueueFamilies(physicalDevice, surface);

	vk::CommandPoolCreateInfo poolInfo = {};
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
	poolInfo.flags = vk::CommandPoolCreateFlagBits(0);

	device.createCommandPool(&poolInfo, nullptr, &commandPool);
}

void HelloTriangleApplication::createVertexBuffer()
{
	vk::DeviceSize size = sizeof(vertices[0]) * vertices.size();
	vk::BufferUsageFlags stagingUsageFlags = vk::BufferUsageFlagBits::eTransferSrc;
	vk::MemoryPropertyFlags properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
	// Staging buffer that we load the data onto that's visible to our CPU
	vk::Buffer stagingBuffer;
	vk::DeviceMemory stagingBufferMemory;

	createBuffer(size, stagingUsageFlags, properties, stagingBuffer, stagingBufferMemory);
	
	
	void* data = device.mapMemory(stagingBufferMemory, 0, size);
		memcpy(data, vertices.data(), (size_t)size);
	device.unmapMemory(stagingBufferMemory);

	// This is our buffer located on our GPU, inaccessible to our CPU
	vk::BufferUsageFlags vertexUsageFlags = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer;
	createBuffer(size, vertexUsageFlags, vk::MemoryPropertyFlagBits::eDeviceLocal, vertexBuffer, vertexBufferMemory);

	copyBuffer(stagingBuffer, vertexBuffer, size);
	
	device.destroyBuffer(stagingBuffer);
	device.freeMemory(stagingBufferMemory);
}

void HelloTriangleApplication::createIndexBuffer()
{
	vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	vk::Buffer stagingBuffer;
	vk::DeviceMemory stagingBufferMemory;
	
	createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

	void* data = device.mapMemory(stagingBufferMemory, 0, bufferSize);
		memcpy(data, indices.data(), (size_t)bufferSize);
	device.unmapMemory(stagingBufferMemory);
	
	vk::BufferUsageFlags indexUsageFlags = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer;
	createBuffer(bufferSize, indexUsageFlags, vk::MemoryPropertyFlagBits::eDeviceLocal, indexBuffer, indexBufferMemory);

	copyBuffer(stagingBuffer, indexBuffer, bufferSize);
	device.destroyBuffer(stagingBuffer);
	device.freeMemory(stagingBufferMemory);
}

void HelloTriangleApplication::createUniformBuffers()
{
	vk::DeviceSize bufferSize = sizeof(TransformationBufferObject);
	auto usageFlags = vk::BufferUsageFlagBits::eUniformBuffer;
	auto properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
	
	size_t numImages = swapchainImages.size();

	uniformBuffers.resize(numImages);
	uniformBuffersMemory.resize(numImages);

	for (size_t i = 0; i < numImages; i++) {
		createBuffer(bufferSize, usageFlags, properties, uniformBuffers[i], uniformBuffersMemory[i]);
	}
}

void HelloTriangleApplication::createDescriptorPool()
{
	uint32_t count = static_cast<uint32_t>(swapchainImages.size());

	vk::DescriptorPoolSize poolSize = {};
	poolSize.type = vk::DescriptorType::eUniformBuffer;
	poolSize.descriptorCount = count;

	vk::DescriptorPoolCreateInfo poolInfo = {};
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = count;

	descriptorPool = device.createDescriptorPool(poolInfo);
}

void HelloTriangleApplication::createDescriptorSets()
{
	size_t count = swapchainImages.size();

	std::vector<vk::DescriptorSetLayout> layouts(count, descriptorSetLayout);
	vk::DescriptorSetAllocateInfo allocInfo = {};
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(count);
	allocInfo.pSetLayouts = layouts.data();

	descriptorSets.resize(count);
	device.allocateDescriptorSets(&allocInfo, &descriptorSets[0]);

	for (size_t i = 0; i < count; i++) {
		vk::DescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = uniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(TransformationBufferObject);
		
		vk::WriteDescriptorSet descriptorWrite = {};
		descriptorWrite.dstSet = descriptorSets[i];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;

		descriptorWrite.descriptorType = vk::DescriptorType::eUniformBuffer;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;

		device.updateDescriptorSets(1, &descriptorWrite, 0, nullptr);
	}
}

void HelloTriangleApplication::createCommandBuffers()
{
	commandBuffers.resize(swapChainFramebuffers.size());

	vk::CommandBufferAllocateInfo allocInfo = {};
	allocInfo.commandPool = commandPool;
	allocInfo.level = vk::CommandBufferLevel::ePrimary;
	allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

	device.allocateCommandBuffers(&allocInfo, commandBuffers.data());

	for (size_t i = 0; i < commandBuffers.size(); i++) {
		vk::CommandBufferBeginInfo beginInfo = {};
		beginInfo.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;

		commandBuffers[i].begin(&beginInfo);

		vk::RenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.renderPass = renderPass;
		renderPassInfo.framebuffer = swapChainFramebuffers[i];

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapchainExtent;

		vk::ClearValue clearColor = vk::ClearColorValue{ std::array<float, 4>{ 0.0f, 0.0f, 0.0f, 1.0f } };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		commandBuffers[i].beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);
		commandBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline);

		vk::Buffer vertexBuffers[] = { vertexBuffer };
		vk::DeviceSize offsets[] = { 0 };
		commandBuffers[i].bindVertexBuffers(0, 1, vertexBuffers, offsets);

		commandBuffers[i].bindIndexBuffer(indexBuffer, 0, vk::IndexType::eUint16);
		commandBuffers[i].bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);

		commandBuffers[i].drawIndexed(static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
		commandBuffers[i].endRenderPass();

		commandBuffers[i].end();
	}
}

void HelloTriangleApplication::createSemaphores()
{
	vk::SemaphoreCreateInfo semaphoreInfo = {};
	
	device.createSemaphore(&semaphoreInfo, nullptr, &imageAvailableSemaphore);
	device.createSemaphore(&semaphoreInfo, nullptr, &renderFinishedSemaphore);
}

void HelloTriangleApplication::recreateSwapChain()
{
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	if (width == 0 || height == 0) return;

	device.waitIdle();

	cleanupSwapChain();

	createSwapChain();
	createImageViews();
	createRenderPass();
	createGraphicsPipeline();
	createFramebuffers();
	createCommandBuffers();
}

void HelloTriangleApplication::createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& bufferMemory)
{
	vk::BufferCreateInfo bufferInfo = {};
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = vk::SharingMode::eExclusive;
	
	device.createBuffer(&bufferInfo, nullptr, &buffer);

	vk::MemoryRequirements memRequirements;
	device.getBufferMemoryRequirements(buffer, &memRequirements);

	vk::MemoryAllocateInfo allocInfo = {};
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	device.allocateMemory(&allocInfo, nullptr, &bufferMemory);
	device.bindBufferMemory(buffer, bufferMemory, 0);
}

void HelloTriangleApplication::copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size)
{
	vk::CommandBufferAllocateInfo allocInfo = {};
	allocInfo.level = vk::CommandBufferLevel::ePrimary;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	vk::CommandBuffer commandBuffer;
	device.allocateCommandBuffers(&allocInfo, &commandBuffer);

	vk::CommandBufferBeginInfo beginInfo = {};
	beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

	commandBuffer.begin(&beginInfo);

	vk::BufferCopy copyRegion = {};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size = size;
	commandBuffer.copyBuffer(srcBuffer, dstBuffer, 1, &copyRegion);

	commandBuffer.end();

	vk::SubmitInfo submitInfo = {};
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	graphicsQueue.submit(1, &submitInfo, vk::Fence{});
	graphicsQueue.waitIdle();

	device.free(commandPool, 1, &commandBuffer);
}

void HelloTriangleApplication::updateUniformBuffer(uint32_t currentImage)
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	TransformationBufferObject ubo = {};
	ubo.model = glm::rotate(glm::mat4{ 1.0f }, time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f), glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.projection = glm::perspective(glm::radians(45.0f), swapchainExtent.width / (float)swapchainExtent.height, 0.1f, 10.0f);
	ubo.projection[1][1] *= -1; // Compensating for the fact GLM was written for OpenGL

	void* data;
	data = device.mapMemory(uniformBuffersMemory[currentImage], 0, sizeof(ubo), (vk::MemoryMapFlagBits)0);
	memcpy(data, &ubo, sizeof(ubo));
	device.unmapMemory(uniformBuffersMemory[currentImage]);
}

vk::ShaderModule HelloTriangleApplication::createShaderModule(const std::vector<char>& code)
{
	vk::ShaderModuleCreateInfo createInfo = {};
	createInfo.codeSize = code.size();
	// This cast is weird, not 100% sure why it's not a static cast.
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
	
	vk::ShaderModule shaderModule;
	device.createShaderModule(&createInfo, nullptr, &shaderModule) ;

	return shaderModule;
}

vk::SurfaceFormatKHR HelloTriangleApplication::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
{
	if (availableFormats.size() == 1 && availableFormats[0].format == vk::Format::eUndefined) {
		return { vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear };
	}

	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == vk::Format::eB8G8R8A8Unorm&& availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
			return availableFormat;
		}
	}

	// If the above two tests fail then we'll just return the first available format
	return availableFormats[0];
}

vk::PresentModeKHR HelloTriangleApplication::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes)
{
	vk::PresentModeKHR bestMode = vk::PresentModeKHR::eFifo;
	for (const auto& presentMode : availablePresentModes) {
		if (presentMode == vk::PresentModeKHR::eMailbox) {
			return presentMode;
		} else if (presentMode == vk::PresentModeKHR::eImmediate) {
			bestMode = presentMode;
		}
	}

	return bestMode;
}

vk::Extent2D HelloTriangleApplication::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR & capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	} else {
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		vk::Extent2D actualExtent = { (uint32_t)width, (uint32_t)height };

		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

uint32_t HelloTriangleApplication::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
{
	// Has two arrays -- memoryTypes and memoryHeaps. Memory Heaps are distinct memory resources like VRAM or swap RAM.
	vk::PhysicalDeviceMemoryProperties memProperties;
	physicalDevice.getMemoryProperties(&memProperties);

	// Check each available memory type
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		// Type filter specifies the bit field of suitable memory types
		bool matchesTypeFilter = static_cast<bool>(typeFilter & (1 << i));
		// Want to also check that the memory types have the required properties.
		bool hasRequiredProperties = ((memProperties.memoryTypes[i].propertyFlags & properties) == properties);
		
		if (matchesTypeFilter && hasRequiredProperties) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

vk::Bool32 HelloTriangleApplication::debugCallback(
	vk::DebugReportFlagsEXT flags,
	vk::DebugReportObjectTypeEXT objType,
	uint64_t obj,
	size_t location,
	int32_t code,
	const char* layerPrefix,
	const char* msg,
	void* userData
)
{
	std::cout << "debug callback" << std::endl;
	std::cerr << "validation layer: " << msg << std::endl;

	// The callback returns a boolean that indicates if the Vulkan call that
	// triggered the validation layer message should be aborted. If the callback
	// returns true, then the call is aborted with the VK_ERROR_VALIDATION_FAILED_EXT
	// error. This is normally only used to test the validation layers themselves,
	// so you should always return VK_FALSE.
	return VK_FALSE;
}

bool HelloTriangleApplication::isDeviceSuitable(const vk::PhysicalDevice& device)
{
	vk::PhysicalDeviceProperties deviceProperties;
	device.getProperties(&deviceProperties);
	
	vk::PhysicalDeviceFeatures deviceFeatures;
	device.getFeatures(&deviceFeatures);

	return (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) &&
		deviceFeatures.geometryShader;
}

std::vector<char> HelloTriangleApplication::readFile(const std::string & filename)
{
	// ate = At the end; this lets us get the filesize easily.
	std::ifstream file{ filename, std::ios::ate | std::ios::binary };

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file");
	}

	size_t fileSize = static_cast<size_t>(file.tellg());
	std::vector<char> buffer(fileSize);

	file.seekg(0); // Return to the beginning
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}

void HelloTriangleApplication::onWindowResize(GLFWwindow * window, int width, int height)
{
	HelloTriangleApplication* app = reinterpret_cast<HelloTriangleApplication*>(glfwGetWindowUserPointer(window));
	app->recreateSwapChain();
}