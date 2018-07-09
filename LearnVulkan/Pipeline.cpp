#include "Pipeline.h"

namespace vkr
{
	namespace GraphicsPipeline
	{

		vk::PipelineShaderStageCreateInfo getVertexShaderStageInfo(const vk::ShaderModule & shaderModule)
		{
			vk::PipelineShaderStageCreateInfo vertShaderStageInfo;
			vertShaderStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
			vertShaderStageInfo.module = shaderModule;
			vertShaderStageInfo.pName = "main";
			return vertShaderStageInfo;
		}
		vk::PipelineShaderStageCreateInfo getFragmentShaderStageInfo(const vk::ShaderModule & shaderModule)
		{
			vk::PipelineShaderStageCreateInfo fragShaderStageInfo;
			fragShaderStageInfo.stage = vk::ShaderStageFlagBits::eFragment;
			fragShaderStageInfo.module = shaderModule;
			fragShaderStageInfo.pName = "main";
			return fragShaderStageInfo;
		}

		vk::PipelineVertexInputStateCreateInfo getVertexInputInfo(
			const vk::VertexInputBindingDescription & bindingDescription,
			const std::vector<vk::VertexInputAttributeDescription> & attributeDescriptions
		)
		{
			vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
			vertexInputInfo.vertexBindingDescriptionCount = 1;
			vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
			vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
			vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
			return vertexInputInfo;
		}

		vk::PipelineInputAssemblyStateCreateInfo getInputAssemblerCreateInfo(const vk::PrimitiveTopology topology)
		{
			vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
			inputAssemblyInfo.topology = topology;
			inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;
			return inputAssemblyInfo;
		}

		vk::PipelineViewportStateCreateInfo getViewportStateCreateInfo(const vk::Extent2D & swapchainExtent)
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

			vk::PipelineViewportStateCreateInfo viewportState;
			viewportState.viewportCount = 1;
			viewportState.pViewports = &viewport;
			viewportState.scissorCount = 1;
			viewportState.pScissors = &scissor;
			return viewportState;
		}

		vk::PipelineRasterizationStateCreateInfo getRasterizerCreateInfo(
			const vk::PolygonMode polygonMode,
			const vk::CullModeFlagBits cullMode,
			const vk::FrontFace frontFace
		)
		{
			vk::PipelineRasterizationStateCreateInfo rasterizer;
			rasterizer.depthClampEnable = VK_FALSE;
			rasterizer.rasterizerDiscardEnable = VK_FALSE;
			rasterizer.polygonMode = polygonMode;
			rasterizer.lineWidth = 1.0f;
			rasterizer.cullMode = cullMode;
			rasterizer.frontFace = frontFace;
			rasterizer.depthBiasEnable = VK_FALSE;
			return rasterizer;
		}

		vk::UniquePipelineLayout createPipelineLayout(
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


		vk::UniquePipeline createPipeline(
			const vk::Device & device,
			const vk::ShaderModule & vertShaderModule,
			const vk::ShaderModule & fragShaderModule,
			const vk::VertexInputBindingDescription & bindingDescription,
			const std::vector<vk::VertexInputAttributeDescription> & attributeDescriptions,
			const vk::Extent2D & swapchainExtent,
			const vk::UniquePipelineLayout & pipelineLayout,
			const vk::RenderPass & renderPass
		)
		{
			vk::PipelineShaderStageCreateInfo vertexStageInfo{ getVertexShaderStageInfo(vertShaderModule) };
			vk::PipelineShaderStageCreateInfo fragStageInfo{ getVertexShaderStageInfo(fragShaderModule) };
			vk::PipelineShaderStageCreateInfo shaderStages[] = { vertexStageInfo, fragStageInfo };

			vk::PipelineVertexInputStateCreateInfo vertexInputInfo{ getVertexInputInfo(bindingDescription, attributeDescriptions) };
			vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo{ getInputAssemblerCreateInfo() };
			vk::PipelineViewportStateCreateInfo viewportState{ getViewportStateCreateInfo(swapchainExtent) };
			vk::PipelineRasterizationStateCreateInfo rasterizerInfo{ getRasterizerCreateInfo() };

			// Create other multisampling state, color blend states, with reasonable defaults for now
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

			vk::GraphicsPipelineCreateInfo pipelineInfo = {};
			pipelineInfo.stageCount = 2;
			pipelineInfo.pStages = shaderStages;
			pipelineInfo.pVertexInputState = &vertexInputInfo;
			pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
			pipelineInfo.pViewportState = &viewportState;
			pipelineInfo.pRasterizationState = &rasterizerInfo;
			pipelineInfo.pMultisampleState = &multisampling;
			pipelineInfo.pColorBlendState = &colorBlending;
			pipelineInfo.layout = pipelineLayout.get();
			pipelineInfo.renderPass = renderPass;
			pipelineInfo.subpass = 0;

			vk::PipelineCache pipelineCache{};

			vk::UniquePipeline pipeline = device.createGraphicsPipelineUnique(pipelineCache, pipelineInfo);
			return pipeline;
		}

		vk::ShaderModule createShaderModule(const vk::Device & device, const std::vector<char>& code)
		{
			vk::ShaderModuleCreateInfo createInfo = {};
			createInfo.codeSize = code.size();
			// This cast is weird, not 100% sure why it's not a static cast.
			createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

			return device.createShaderModule(createInfo);
		}
	}
};