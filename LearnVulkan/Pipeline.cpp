#include "Pipeline.h"

namespace bvk
{
	GraphicsPipeline::GraphicsPipeline(GraphicsPipeline && otherPipeline) : _pipeline(otherPipeline._pipeline)
	{
		otherPipeline._pipeline = vk::UniquePipeline();
	}

	GraphicsPipeline & GraphicsPipeline::operator=(GraphicsPipeline && otherPipeline)
	{
		if (this != &otherPipeline) {
			std::swap(_pipeline, otherPipeline._pipeline);
		}
	}

	void GraphicsPipeline::addVertexShaderStage(vk::ShaderModule shaderModule, vk::VertexInputBindingDescription bindingDescription, std::vector<vk::VertexInputAttributeDescription> attributeDescriptions)
	{
		vertShaderStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
		vertShaderStageInfo.module = shaderModule;
		vertShaderStageInfo.pName = "main";

		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
	}

	void GraphicsPipeline::addFragmentShaderStage(vk::ShaderModule shaderModule)
	{
		fragShaderStageInfo.stage = vk::ShaderStageFlagBits::eFragment;
		fragShaderStageInfo.module = shaderModule;
		fragShaderStageInfo.pName = "main";
	}

	void GraphicsPipeline::addViewportState(const vk::Extent2D & swapchainExtent)
	{
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

		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;
	}

	void GraphicsPipeline::addRasterizer(
		const vk::PolygonMode polygonMode = vk::PolygonMode::eFill,
		const vk::CullModeFlagBits cullMode = vk::CullModeFlagBits::eBack,
		const vk::FrontFace frontFace = vk::FrontFace::eCounterClockwise
	)
	{
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = polygonMode;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = cullMode;
		rasterizer.frontFace = frontFace;
		rasterizer.depthBiasEnable = VK_FALSE;
	}

	void GraphicsPipeline::addInputAssembler(vk::PrimitiveTopology topology = vk::PrimitiveTopology::eTriangleList)
	{
		inputAssemblyInfo.topology = topology;
		inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;
	}


	void GraphicsPipeline::createPipeline(const vk::Device & device, const vk::DescriptorSetLayout & descriptorSetLayout, const vk::RenderPass & renderPass)
	{
		vk::PipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

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

		_pipelineLayout = device.createPipelineLayoutUnique(pipelineLayoutInfo);

		vk::GraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.layout = _pipelineLayout;
		pipelineInfo.renderPass = renderPass;
		pipelineInfo.subpass = 0;

		_pipeline = std::move(device.createGraphicsPipelinesUnique(vk::PipelineCache{}, std::vector<vk::GraphicsPipelineCreateInfo>{pipelineInfo})[0]);

	}

	vk::ShaderModule GraphicsPipeline::createShaderModule(const vk::Device & device, const std::vector<char>& code)
	{
		vk::ShaderModuleCreateInfo createInfo = {};
		createInfo.codeSize = code.size();
		// This cast is weird, not 100% sure why it's not a static cast.
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		return device.createShaderModule(createInfo);
	}
}

;