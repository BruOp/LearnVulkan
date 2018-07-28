#pragma once
#include <vulkan/vulkan.hpp>


namespace vkr
{
    namespace CommandBufferUtils
    {
        vk::CommandBuffer createOneTimeCommand(const vk::Device & device, const vk::CommandPool commandPool);

        void endCommandBuffer(
            vk::CommandBuffer & commandBuffer,
            const vk::Device & device,
            const vk::CommandPool commandPool,
            const vk::Queue queue
        );
    };
}