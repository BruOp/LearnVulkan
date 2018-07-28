#include "CommandBufferUtils.h"

namespace vkr
{
    vk::CommandBuffer CommandBufferUtils::createOneTimeCommand(
        const vk::Device & device,
        const vk::CommandPool commandPool
    )
    {
        vk::CommandBufferAllocateInfo allocInfo = {};
        allocInfo.level = vk::CommandBufferLevel::ePrimary;
        allocInfo.commandPool = commandPool;
        allocInfo.commandBufferCount = 1;

        vk::CommandBuffer cmdBuffer;
        device.allocateCommandBuffers(&allocInfo, &cmdBuffer);

        vk::CommandBufferBeginInfo beginInfo = {};
        beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

        cmdBuffer.begin(&beginInfo);
        return cmdBuffer;
    }

    void CommandBufferUtils::endCommandBuffer(
        vk::CommandBuffer & cmdBuffer,
        const vk::Device & device,
        const vk::CommandPool commandPool,
        const vk::Queue queue
    )
    {
        cmdBuffer.end();

        vk::SubmitInfo submitInfo = {};
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &cmdBuffer;

        // Create fence to ensure that the command buffer has finished executing
        vk::FenceCreateInfo fenceInfo{};
        vk::Fence fence = device.createFence(fenceInfo);

        // Submit to the queue
        queue.submit(1, &submitInfo, fence);
        queue.waitIdle();
        device.destroyFence(fence);

        device.free(commandPool, 1, &cmdBuffer);
    }
}

