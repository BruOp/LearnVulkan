#pragma once
#include <vector>
#include <utility>
#include <vulkan/vulkan.hpp>

namespace vkr
{
	class GraphicsPipelineFactory
	{
	public:
		GraphicsPipelineFactory(
			const vk::VertexInputBindingDescription & bindingDescription,
			const std::vector<vk::VertexInputAttributeDescription> & attributeDescriptions,
			const vk::Extent2D & swapchainExtent
		);

		void addShaderStage(const vk::ShaderModule & shaderModule, vk::ShaderStageFlagBits stageFlag);

		vk::UniquePipeline createPipeline(
			const vk::Device & device,
			const vk::ShaderModule & vertShaderModule,
			const vk::ShaderModule & fragShaderModule,
			const vk::UniquePipelineLayout & pipelineLayout,
			const vk::RenderPass & renderPass
		);

		void setVertexInputInfo(
			const vk::VertexInputBindingDescription & bindingDescription,
			const std::vector<vk::VertexInputAttributeDescription> & attributeDescriptions
		);

		void setInputAssemblerCreateInfo(
			vk::PrimitiveTopology topology = vk::PrimitiveTopology::eTriangleList
		);

		void setViewportStateCreateInfo(const vk::Extent2D & swapchainExtent);

		void setRasterizerCreateInfo(
			const vk::PolygonMode polygonMode = vk::PolygonMode::eFill,
			const vk::CullModeFlagBits cullMode = vk::CullModeFlagBits::eBack,
			const vk::FrontFace frontFace = vk::FrontFace::eCounterClockwise
		);

		static vk::UniquePipelineLayout createPipelineLayout(
			const vk::Device & device,
			const vk::DescriptorSetLayout & descriptorSetLayout
		);

		static vk::ShaderModule createShaderModule(const vk::Device& device, const std::vector<char>& code);

	private:
		std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;

		vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
		vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo;

		vk::Viewport viewport;
		vk::Rect2D scissor;
		vk::PipelineViewportStateCreateInfo viewportStateInfo;
		vk::PipelineRasterizationStateCreateInfo rasterizerInfo;

		vk::PipelineMultisampleStateCreateInfo multisamplingInfo;
		vk::PipelineColorBlendAttachmentState colorBlendAttachment;
		vk::PipelineColorBlendStateCreateInfo colorBlending;

		void setColorBlendingInfo();
	};


}