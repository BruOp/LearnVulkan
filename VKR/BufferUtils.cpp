#include "BufferUtils.h"


namespace vkr
{
    namespace BufferUtils
    {
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
