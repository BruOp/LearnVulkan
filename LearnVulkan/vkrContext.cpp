#include "vkrContext.h"

const std::vector<const char*> vkrContext::validationLayers = {
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
    { { -0.5f, -0.5f, 0.5f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
	{ {  0.5f, -0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },
	{ {  0.5f,  0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } },
	{ { -0.5f,  0.5f, 0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } }
};

const std::vector<uint16_t> indices = {
	0, 1, 2, 2, 3, 0
};

vkrContext::vkrContext(int width, int height) :
	width(width),
	height(height)
{
}

void vkrContext::run()
{
	initWindow();
	initVulkan();
	mainLoop();
	cleanup();
}

void vkrContext::initWindow()
{
	vkr::Window::initializeGLFW();
	window = vkr::Window(width, height);
	window.setResizeCallback(this, vkrContext::onWindowResize);
}

void vkrContext::initVulkan()
{
	createInstance();
	setupDebugCallback();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
    initCommandManager();
	createAllocator();
	createSwapChain();
	createImageViews();
	createRenderPass();
	createDescriptorSetLayout();
	createGraphicsPipeline();
	createFramebuffers();
    createTextureImage();
	createVertexBuffer();
	createIndexBuffer();
	createUniformBuffers();
	createDescriptorPool();
	createDescriptorSets();
	createCommandBuffers();
	createSemaphores();
}

void vkrContext::mainLoop()
{
	while (!window.shouldClose()) {
		vkr::Window::pollWindowEvents();
		drawFrame();
	}
	device.waitIdle();
}

void vkrContext::drawFrame()
{
	//  Acquire an image from the swap chain
	//	Execute the command buffer with that image as attachment in the framebuffer
	//	Return the image to the swap chain for presentation
	uint32_t imageIndex;
	commandManager._presentQueue.waitIdle();

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

	commandManager._graphicsQueue.submit(1, &submitInfo, vk::Fence{});

	vk::PresentInfoKHR presentInfo = {};
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	vk::SwapchainKHR swapChains[] = { swapchain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;
	result = commandManager._presentQueue.presentKHR(&presentInfo);

	if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR) {
		recreateSwapChain();
	} else if (result != vk::Result::eSuccess) {
		throw std::runtime_error("failed to present swap chain image!");
	}
}

void vkrContext::cleanup()
{
	cleanupSwapChain();

    texture.destroy(device, vulkanAllocator);

	device.destroyDescriptorPool(descriptorPool);
	device.destroyDescriptorSetLayout(descriptorSetLayout);

	for (size_t i = 0; i < swapchainImages.size(); i++) {
		uniformBuffers[i].destroy(vulkanAllocator);
	}

    vertexBuffer.destroy(vulkanAllocator);
	indexBuffer.destroy(vulkanAllocator);

	device.destroySemaphore(renderFinishedSemaphore);
	device.destroySemaphore(imageAvailableSemaphore);

	vmaDestroyAllocator(vulkanAllocator);

	device.destroy();
	DestroyDebugReportCallbackEXT(instance, callback, nullptr);
	instance.destroySurfaceKHR(surface);
	instance.destroy();
	window.destroy();

	vkr::Window::terminateGLFW();
}

void vkrContext::cleanupSwapChain()
{
	for (auto & framebuffer : swapChainFramebuffers) {
		device.destroyFramebuffer(framebuffer);
	}

    commandManager.freeCommandBuffers(commandBuffers);

	graphicsPipeline = vk::UniquePipeline();
	pipelineLayout = vk::UniquePipelineLayout();

	device.destroyRenderPass(renderPass);

	for (auto imageView : swapchainImageViews) {
		device.destroyImageView(imageView);
	}

	swapchain.destroy(device);
}

void vkrContext::createInstance()
{
	instance = vkr::InstanceFactory::create(validationLayers);
}

void vkrContext::pickPhysicalDevice()
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

void vkrContext::createSurface()
{
	surface = window.createWindowSurface(instance);
}

void vkrContext::createLogicalDevice()
{
	vkr::LogicalDeviceFactory factory{ physicalDevice, surface };

    device = factory.create(physicalDevice, surface);
}

void vkrContext::initCommandManager()
{
    vkr::QueueFamilyChecker checker{ physicalDevice, surface };
    commandManager = vkr::CommandManager{ device, checker };
}

void vkrContext::createAllocator()
{
	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.physicalDevice = physicalDevice;
	allocatorInfo.device = device;

	vmaCreateAllocator(&allocatorInfo, &vulkanAllocator);
}

void vkrContext::setupDebugCallback()
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

void vkrContext::createSwapChain()
{
	swapchain = vkr::SwapChain{ device, physicalDevice, surface, window };
	swapchain.getSwapChainImages(device, swapchainImages);
}

void vkrContext::createImageViews()
{
	swapchainImageViews.resize(swapchainImages.size());

	for (size_t i = 0; i < swapchainImages.size(); i++) {
        swapchainImageViews[i] = vkr::ImageView(device, swapchainImages[i], vk::Format::eB8G8R8A8Unorm);
	}
}

void vkrContext::createRenderPass()
{
	vk::AttachmentDescription colorAttachment = {};
	colorAttachment.format = swapchain.swapChainImageFormat.format;
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

void vkrContext::createDescriptorSetLayout()
{
    vk::DescriptorSetLayoutBinding uboLayoutBinding = {};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eVertex;

    vk::DescriptorSetLayoutBinding samplerLayoutBinding = {
        1, // binding
        vk::DescriptorType::eCombinedImageSampler, // Descriptor type
        1, //descriptor count
        vk::ShaderStageFlagBits::eFragment
    };

    std::vector<vk::DescriptorSetLayoutBinding> bindings{ uboLayoutBinding, samplerLayoutBinding };
    vk::DescriptorSetLayoutCreateInfo layoutInfo{
        vk::DescriptorSetLayoutCreateFlags{},
        (uint32_t)bindings.size(),
        bindings.data()
    };

    descriptorSetLayout = device.createDescriptorSetLayout(layoutInfo);
}

void vkrContext::createGraphicsPipeline()
{
	auto vertShaderCode = readFile("shaders/vert.spv");
	auto fragShaderCode = readFile("shaders/frag.spv");

	vk::ShaderModule vertShaderModule = createShaderModule(vertShaderCode);
	vk::ShaderModule fragShaderModule = createShaderModule(fragShaderCode);

	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescriptions = Vertex::getAttributeDescriptions();

	pipelineLayout = vkr::GraphicsPipelineFactory::createPipelineLayout(device, descriptorSetLayout);

	vkr::GraphicsPipelineFactory factory{ bindingDescription, attributeDescriptions, swapchain.swapChainExtent };
	graphicsPipeline = factory.createPipeline(
		device,
		vertShaderModule,
		fragShaderModule,
		pipelineLayout,
		renderPass
	);
}

void vkrContext::createFramebuffers()
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
		framebufferInfo.width = swapchain.swapChainExtent.width;
		framebufferInfo.height = swapchain.swapChainExtent.height;
		framebufferInfo.layers = 1;

		device.createFramebuffer(&framebufferInfo, nullptr, &swapChainFramebuffers[i]);
	}
}

void vkrContext::createTextureImage()
{
    texture = vkr::Texture(device, vulkanAllocator, commandManager, "images/texture.jpg");
}

void vkrContext::createVertexBuffer()
{
	vk::DeviceSize size = sizeof(vertices[0]) * vertices.size();
	vertexBuffer = vkr::BufferUtils::create(commandManager, vk::BufferUsageFlagBits::eVertexBuffer, size, vertices, vulkanAllocator);
}

void vkrContext::createIndexBuffer()
{
    vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    indexBuffer = vkr::BufferUtils::create(commandManager, vk::BufferUsageFlagBits::eIndexBuffer, bufferSize, indices, vulkanAllocator);
}

void vkrContext::createUniformBuffers()
{
	vk::DeviceSize bufferSize = sizeof(TransformationBufferObject);
	auto usageFlags = vk::BufferUsageFlagBits::eUniformBuffer;

	size_t numImages = swapchainImages.size();

	uniformBuffers.resize(numImages);

	for (size_t i = 0; i < numImages; i++) {
		uniformBuffers[i] = vkr::Buffer{ vulkanAllocator, bufferSize, usageFlags, VMA_MEMORY_USAGE_CPU_TO_GPU };
	}
}

void vkrContext::createDescriptorPool()
{
	uint32_t count = static_cast<uint32_t>(swapchainImages.size());

	std::vector<vk::DescriptorPoolSize> poolSize(2);

    poolSize[0].type = vk::DescriptorType::eUniformBuffer;
	poolSize[0].descriptorCount = count;
    poolSize[1].type = vk::DescriptorType::eCombinedImageSampler;
    poolSize[1].descriptorCount = count;

	vk::DescriptorPoolCreateInfo poolInfo = {};
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSize.size());
	poolInfo.pPoolSizes = poolSize.data();
	poolInfo.maxSets = count;

	descriptorPool = device.createDescriptorPool(poolInfo);
}

void vkrContext::createDescriptorSets()
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
		bufferInfo.buffer = uniformBuffers[i].get();
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(TransformationBufferObject);

        vk::DescriptorImageInfo imageInfo{ texture.getImageInfo() };

        std::vector<vk::WriteDescriptorSet> descriptorWrites{2};

		descriptorWrites[0].dstSet = descriptorSets[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = vk::DescriptorType::eUniformBuffer;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;

        descriptorWrites[1].dstSet = descriptorSets[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = vk::DescriptorType::eCombinedImageSampler;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;

        device.updateDescriptorSets(descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
	}
}

void vkrContext::createCommandBuffers()
{
	commandBuffers.resize(swapChainFramebuffers.size());

	vk::CommandBufferAllocateInfo allocInfo = {};
	allocInfo.commandPool = commandManager._commandPool;
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
		renderPassInfo.renderArea.extent = swapchain.swapChainExtent;

		vk::ClearValue clearColor = vk::ClearColorValue{ std::array<float, 4>{ 0.0f, 0.0f, 0.0f, 1.0f } };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		commandBuffers[i].beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);
		commandBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline.get());

		vk::Buffer vertexBuffers[] = { vertexBuffer };
		vk::DeviceSize offsets[] = { 0 };
		commandBuffers[i].bindVertexBuffers(0, 1, vertexBuffers, offsets);

		commandBuffers[i].bindIndexBuffer(indexBuffer, 0, vk::IndexType::eUint16);
		commandBuffers[i].bindDescriptorSets(
			vk::PipelineBindPoint::eGraphics,
			pipelineLayout.get(),
			0,
			1,
			&descriptorSets[i],
			0,
			nullptr
		);

		commandBuffers[i].drawIndexed(static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
		commandBuffers[i].endRenderPass();

		commandBuffers[i].end();
	}
}

void vkrContext::createSemaphores()
{
	vk::SemaphoreCreateInfo semaphoreInfo = {};

	device.createSemaphore(&semaphoreInfo, nullptr, &imageAvailableSemaphore);
	device.createSemaphore(&semaphoreInfo, nullptr, &renderFinishedSemaphore);
}

void vkrContext::recreateSwapChain()
{
	if (window.getWidth() == 0 || window.getHeight() == 0) return;

	device.waitIdle();

	cleanupSwapChain();

	createSwapChain();
	createImageViews();
	createRenderPass();
	createGraphicsPipeline();
	createFramebuffers();
	createCommandBuffers();
}

void vkrContext::updateUniformBuffer(uint32_t currentImage)
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	TransformationBufferObject ubo = {};
	ubo.model = glm::rotate(glm::mat4{ 1.0f }, time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f), glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.projection = glm::perspective(glm::radians(45.0f), swapchain.swapChainExtent.width / (float)swapchain.swapChainExtent.height, 0.1f, 10.0f);
	ubo.projection[1][1] *= -1; // Compensating for the fact GLM was written for OpenGL

	uniformBuffers[currentImage].copyInto(vulkanAllocator, &ubo);
}

vk::ShaderModule vkrContext::createShaderModule(const std::vector<char>& code)
{
	vk::ShaderModuleCreateInfo createInfo = {};
	createInfo.codeSize = code.size();
	// This cast is weird, not 100% sure why it's not a static cast.
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	vk::ShaderModule shaderModule;
	device.createShaderModule(&createInfo, nullptr, &shaderModule) ;

	return shaderModule;
}

uint32_t vkrContext::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
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

vk::Bool32 vkrContext::debugCallback(
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

bool vkrContext::isDeviceSuitable(const vk::PhysicalDevice& device)
{
	vk::PhysicalDeviceProperties deviceProperties;
	device.getProperties(&deviceProperties);

    vk::PhysicalDeviceFeatures deviceFeatures{ device.getFeatures() };

	return (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
        && deviceFeatures.samplerAnisotropy;
}

std::vector<char> vkrContext::readFile(const std::string & filename)
{
	// ate = At the end; this lets us get the filesize easily.
	std::ifstream file{ filename, std::ios::ate | std::ios::binary };

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file" + filename);
	}

	size_t fileSize = static_cast<size_t>(file.tellg());
	std::vector<char> buffer(fileSize);

	file.seekg(0); // Return to the beginning
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}

void vkrContext::onWindowResize(GLFWwindow * window, int width, int height)
{
	vkrContext* app = reinterpret_cast<vkrContext*>(glfwGetWindowUserPointer(window));
	app->recreateSwapChain();
}