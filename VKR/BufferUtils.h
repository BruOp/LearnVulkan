#pragma once

#include "Buffer.h"
#include "CommandManager.h"

namespace vkr {
    namespace BufferUtils
    {
        template<typename ArrayProxy>
        vkr::Buffer create(
            vkr::CommandManager & commandManager,
            const vk::BufferUsageFlags usageFlags,
            const vk::DeviceSize size,
            const ArrayProxy & data,
            const VmaAllocator & _allocator
        )
        {
            // Staging buffer that we load the data onto that's visible to our CPU
            vk::BufferUsageFlags stagingUsageFlags = vk::BufferUsageFlagBits::eTransferSrc;
            vkr::Buffer stagingBuffer{ _allocator, size, stagingUsageFlags, VMA_MEMORY_USAGE_CPU_ONLY };
            stagingBuffer.copyInto(_allocator, data.data());

            // This is our buffer located on our GPU, inaccessible to our CPU
            vk::BufferUsageFlags gpuBufferUsageFlags = vk::BufferUsageFlagBits::eTransferDst | usageFlags;
            vkr::Buffer gpuBuffer{ _allocator, size, gpuBufferUsageFlags, VMA_MEMORY_USAGE_GPU_ONLY };

            copyBuffer(commandManager, stagingBuffer, gpuBuffer);

            stagingBuffer.destroy(_allocator);

            return gpuBuffer;
        };

        void copyBuffer(
            vkr::CommandManager & commandManager,
            vkr::Buffer& stagingBuffer,
            vkr::Buffer& gpuBuffer
        );
    };
}