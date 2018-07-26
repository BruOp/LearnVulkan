#pragma once

#include "Buffer.h"

namespace vkr {
    namespace BufferUtils
    {
        vkr::Buffer create(
            vk::Device & device,
            vk::CommandPool & commandPool,
            vk::Queue & graphicsQueue,
            const vk::BufferUsageFlags usageFlags,
            const vk::DeviceSize size,
            const void * dataSrc,
            const VmaAllocator & _allocator
        );

        void copyBuffer(
            vk::Device & device,
            vk::CommandPool & commandPool,
            vk::Queue & graphicsQueue,
            vkr::Buffer& stagingBuffer,
            vkr::Buffer& gpuBuffer
        );
    };
}