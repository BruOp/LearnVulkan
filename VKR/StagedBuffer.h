#pragma once

#include "Buffer.h"

namespace vkr {
    class StagedBufferFactory
    {
    public:
        vkr::Buffer create(
        const vk::Device & device,
            const vk::CommandPool & commandPool,
            const vk::Queue & graphicsQueue,
            const vk::BufferUsageFlags usageFlags,
            const vk::DeviceSize size,
            const void * dataSrc,
            const VmaAllocator & _allocator
        );

    private:
        vkr::Buffer stagingBuffer;

        void copyBuffer(
            const vk::Device & device,
            const vk::CommandPool & commandPool,
            const vk::Queue & graphicsQueue,
            const vkr::Buffer& gpuBuffer
        );
    };
}