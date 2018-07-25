#include "StagedBuffer.h"


namespace vkr
{
    namespace StagedBufferFactory
    {
        vkr::Buffer create(
            vk::Device & device,
            vk::CommandPool & commandPool,
            vk::Queue & graphicsQueue,
            const vk::BufferUsageFlags usageFlags,
            const vk::DeviceSize size,
            const void * dataSrc,
            const VmaAllocator & _allocator
        )
        {
            // Staging buffer that we load the data onto that's visible to our CPU
            vk::BufferUsageFlags stagingUsageFlags = vk::BufferUsageFlagBits::eTransferSrc;
            vkr::Buffer stagingBuffer{ _allocator, size, stagingUsageFlags, VMA_MEMORY_USAGE_CPU_ONLY };
            stagingBuffer.copyInto(_allocator, dataSrc);

            // This is our buffer located on our GPU, inaccessible to our CPU
            vk::BufferUsageFlags gpuBufferUsageFlags = vk::BufferUsageFlagBits::eTransferDst | usageFlags;
            vkr::Buffer gpuBuffer{ _allocator, size, gpuBufferUsageFlags, VMA_MEMORY_USAGE_GPU_ONLY };

            copyBuffer(device, commandPool, graphicsQueue, stagingBuffer, gpuBuffer);

            stagingBuffer.destroy(_allocator);

            return gpuBuffer;
        }

        void copyBuffer(
            vk::Device & device,
            vk::CommandPool & commandPool,
            vk::Queue & graphicsQueue,
            vkr::Buffer & stagingBuffer,
            vkr::Buffer & gpuBuffer
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
            copyRegion.size = stagingBuffer.size;
            commandBuffer.copyBuffer(stagingBuffer.buffer, gpuBuffer.buffer, 1, &copyRegion);

            commandBuffer.end();

            vk::SubmitInfo submitInfo = {};
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &commandBuffer;

            // Create fence to ensure that the command buffer has finished executing
            vk::FenceCreateInfo fenceInfo{};
            vk::Fence fence = device.createFence(fenceInfo);

            // Submit to the queue
            graphicsQueue.submit(1, &submitInfo, fence);
            graphicsQueue.waitIdle();
            //device.waitForFences({ fence }, true, 100000000000);
            device.destroyFence(fence);

            device.free(commandPool, 1, &commandBuffer);
        }
    }
}
