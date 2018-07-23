#include "StagedBuffer.h"


namespace vkr
{
	vkr::Buffer StagedBufferFactory::create(
		const vk::Device & device,
		const vk::CommandPool & commandPool,
		const vk::Queue & graphicsQueue,
		const vk::BufferUsageFlags usageFlags,
		const vk::DeviceSize size,
		const void * dataSrc,
		const VmaAllocator & _allocator
	)
	{
		vk::BufferUsageFlags stagingUsageFlags = vk::BufferUsageFlagBits::eTransferSrc;
		// Staging buffer that we load the data onto that's visible to our CPU
		stagingBuffer = Buffer{ _allocator, size, stagingUsageFlags, VMA_MEMORY_USAGE_CPU_TO_GPU };
		stagingBuffer.copyInto(_allocator, &dataSrc, (size_t)size);

		// This is our buffer located on our GPU, inaccessible to our CPU
		vk::BufferUsageFlags gpuBufferUsageFlags = vk::BufferUsageFlagBits::eTransferDst | usageFlags;
		vkr::Buffer gpuBuffer{ _allocator, size, gpuBufferUsageFlags, VMA_MEMORY_USAGE_GPU_ONLY };

		copyBuffer(device, commandPool, graphicsQueue, gpuBuffer);

		stagingBuffer.destroy(_allocator);
		return gpuBuffer;
	}

	void StagedBufferFactory::copyBuffer(
		const vk::Device & device,
		const vk::CommandPool & commandPool,
		const vk::Queue & graphicsQueue,
		const vkr::Buffer & gpuBuffer
	)
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
		copyRegion.size = gpuBuffer.size;
		commandBuffer.copyBuffer(stagingBuffer, gpuBuffer, 1, &copyRegion);

		commandBuffer.end();

		vk::SubmitInfo submitInfo = {};
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		graphicsQueue.submit(1, &submitInfo, vk::Fence{});
		graphicsQueue.waitIdle();

		device.free(commandPool, 1, &commandBuffer);
	}
}
