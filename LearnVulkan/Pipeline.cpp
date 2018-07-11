#include "Pipeline.h"

namespace vkr
{
	GraphicsPipelineFactory::GraphicsPipelineFactory(
		const vk::VertexInputBindingDescription & bindingDescription,
		const std::vector<vk::VertexInputAttributeDescription>& attributeDescriptions,
		const vk::Extent2D & swapchainExtent
	)
	{
		setVertexInputInfo(bindingDescription, attributeDescriptions);
		setInputAssemblerCreateInfo();
		setViewportStateCreateInfo(swapchainExtent);
		setRasterizerCreateInfo();
		setColorBlendingInfo();
	}

	void GraphicsPipelineFactory::addShaderStage(const vk::ShaderModule & shaderModule, vk::ShaderStageFlagBits stageFlag)
	{
		vk::PipelineShaderStageCreateInfo shaderInfo{ vk::PipelineShaderStageCreateFlags() , stageFlag, shaderModule, "main" };
		shaderStages.push_back(shaderInfo);
	}

	vk::UniquePipeline GraphicsPipelineFactory::createPipeline(
		const vk::Device & device,
		const vk::ShaderModule & vertShaderModule,
		const vk::ShaderModule & fragShaderModule,
		const vk::UniquePipelineLayout & pipelineLayout,
		const vk::RenderPass & renderPass
	)
	{
		addShaderStage(vertShaderModule, vk::ShaderStageFlagBits::eVertex);
		addShaderStage(fragShaderModule, vk::ShaderStageFlagBits::eFragment);
		
		// Create other multisampling state, color blend states, with reasonable defaults for now
		multisamplingInfo.sampleShadingEnable = VK_FALSE;
		multisamplingInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;

		vk::GraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages.data();
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
		pipelineInfo.pViewportState = &viewportStateInfo;
		pipelineInfo.pRasterizationState = &rasterizerInfo;
		pipelineInfo.pMultisampleState = &multisamplingInfo;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.layout = pipelineLayout.get();
		pipelineInfo.renderPass = renderPass;
		pipelineInfo.subpass = 0;

		vk::UniquePipeline pipeline = device.createGraphicsPipelineUnique(vk::PipelineCache{}, pipelineInfo);
		return pipeline;
	}


	vk::UniquePipelineLayout GraphicsPipelineFactory::createPipelineLayout(
		const vk::Device & device,
		const vk::DescriptorSetLayout & descriptorSetLayout
	)
	{
		vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
		pipelineLayoutInfo.pushConstantRangeCount = 0;

		vk::UniquePipelineLayout pipelineLayout = device.createPipelineLayoutUnique(pipelineLayoutInfo);
		return pipelineLayout;
	}


	vk::ShaderModule GraphicsPipelineFactory::createShaderModule(const vk::Device & device, const std::vector<char>& code)
	{
		vk::ShaderModuleCreateInfo createInfo = {};
		createInfo.codeSize = code.size();
		// This cast is weird, not 100% sure why it's not a static cast.
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		return device.createShaderModule(createInfo);
	}

	void GraphicsPipelineFactory::setVertexInputInfo(const vk::VertexInputBindingDescription & bindingDescription, const std::vector<vk::VertexInputAttributeDescription>& attributeDescriptions)
	{
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
	}

	void GraphicsPipelineFactory::setInputAssemblerCreateInfo(vk::PrimitiveTopology topology)
	{
		inputAssemblyInfo.topology = topology;
		inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;
	}

	void GraphicsPipelineFactory::setViewportStateCreateInfo(const vk::Extent2D & swapchainExtent)
	{
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)swapchainExtent.width;
		viewport.height = (float)swapchainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		
		scissor.offset = { 0, 0 };
		scissor.extent = swapchainExtent;

		viewportStateInfo.viewportCount = 1;
		viewportStateInfo.pViewports = &viewport;
		viewportStateInfo.scissorCount = 1;
		viewportStateInfo.pScissors = &scissor;
	}

	void GraphicsPipelineFactory::setRasterizerCreateInfo(const vk::PolygonMode polygonMode, const vk::CullModeFlagBits cullMode, const vk::FrontFace frontFace)
	{
		rasterizerInfo.depthClampEnable = VK_FALSE;
		rasterizerInfo.rasterizerDiscardEnable = VK_FALSE;
		rasterizerInfo.polygonMode = polygonMode;
		rasterizerInfo.lineWidth = 1.0f;
		rasterizerInfo.cullMode = cullMode;
		rasterizerInfo.frontFace = frontFace;
		rasterizerInfo.depthBiasEnable = VK_FALSE;
	}

	void GraphicsPipelineFactory::setColorBlendingInfo()
	{
		colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
		colorBlendAttachment.blendEnable = VK_FALSE;

		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = vk::LogicOp::eCopy;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;
	}
	
};