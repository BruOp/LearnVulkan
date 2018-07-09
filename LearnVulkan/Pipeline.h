#pragma once
#include <vector>
#include <utility>
#include <vulkan/vulkan.hpp>

namespace bvk
{
	class GraphicsPipeline
	{
	public:
		GraphicsPipeline();
		~GraphicsPipeline() = default;
		// No copying allowed
		GraphicsPipeline(const GraphicsPipeline&) = delete;
		GraphicsPipeline & operator=(const GraphicsPipeline&) = delete;
		
		// Moving is permitted
		GraphicsPipeline(GraphicsPipeline&& pipeline);
		GraphicsPipeline& operator=(GraphicsPipeline&& pipeline);

		void addInputAssembler(vk::PrimitiveTopology topology);

		void addVertexShaderStage(
			vk::ShaderModule shaderModule,
			vk::VertexInputBindingDescription bindingDescription,
			std::vector<vk::VertexInputAttributeDescription> attributeDescriptions
		);

		void addFragmentShaderStage(vk::ShaderModule shaderModule);

		void addViewportState(const vk::Extent2D & swapchainExtent);
		void addRasterizer(
			const vk::PolygonMode polygonMode = vk::PolygonMode::eFill,
			const vk::CullModeFlagBits cullMode = vk::CullModeFlagBits::eBack,
			const vk::FrontFace frontFace = vk::FrontFace::eCounterClockwise
		);


	private:
		vk::UniquePipeline _pipeline;
		vk::UniquePipelineLayout _pipelineLayout;
		
		vk::PipelineShaderStageCreateInfo vertShaderStageInfo;
		vk::PipelineShaderStageCreateInfo fragShaderStageInfo;
		vk::PipelineVertexInputStateCreateInfo vertexInputInfo;

		vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		vk::PipelineViewportStateCreateInfo viewportState;

		vk::PipelineRasterizationStateCreateInfo rasterizer;

		void createPipeline(const vk::Device & device, const vk::DescriptorSetLayout & descriptorSetLayout, const vk::RenderPass & renderPass);
		vk::ShaderModule createShaderModule(const vk::Device& device, const std::vector<char>& code);

	};


}