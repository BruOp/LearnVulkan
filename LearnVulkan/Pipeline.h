#pragma once
#include <vector>
#include <utility>
#include <vulkan/vulkan.hpp>

namespace vkr
{
	namespace GraphicsPipeline
	{
		vk::PipelineShaderStageCreateInfo getVertexShaderStageInfo(const vk::ShaderModule & shaderModule);
		vk::PipelineShaderStageCreateInfo getFragmentShaderStageInfo(const vk::ShaderModule & shaderModule);

		vk::PipelineVertexInputStateCreateInfo getVertexInputInfo(
			const vk::VertexInputBindingDescription & bindingDescription,
			const std::vector<vk::VertexInputAttributeDescription> & attributeDescriptions
		);

		vk::PipelineInputAssemblyStateCreateInfo getInputAssemblerCreateInfo(
			vk::PrimitiveTopology topology = vk::PrimitiveTopology::eTriangleList
		);
		
		vk::PipelineViewportStateCreateInfo getViewportStateCreateInfo(const vk::Extent2D & swapchainExtent);
		
		vk::PipelineRasterizationStateCreateInfo getRasterizerCreateInfo(
			const vk::PolygonMode polygonMode = vk::PolygonMode::eFill,
			const vk::CullModeFlagBits cullMode = vk::CullModeFlagBits::eBack,
			const vk::FrontFace frontFace = vk::FrontFace::eCounterClockwise
		);
		
		vk::UniquePipelineLayout createPipelineLayout(
			const vk::Device & device,
			const vk::DescriptorSetLayout & descriptorSetLayout
		);
		
		vk::UniquePipeline createPipeline(
			const vk::Device & device,
			const vk::ShaderModule & vertShaderModule,
			const vk::ShaderModule & fragShaderModule,
			const vk::VertexInputBindingDescription & bindingDescription,
			const std::vector<vk::VertexInputAttributeDescription> & attributeDescriptions,
			const vk::Extent2D & swapchainExtent,
			const vk::UniquePipelineLayout & pipelineLayout,
			const vk::RenderPass & renderPass
		);
		
		vk::ShaderModule createShaderModule(const vk::Device& device, const std::vector<char>& code);
	};


}