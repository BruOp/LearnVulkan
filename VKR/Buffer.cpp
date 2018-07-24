#include "Buffer.h"

namespace vkr
{
    Buffer::Buffer() : buffer(VK_NULL_HANDLE), size(0) {}

    Buffer::Buffer(
        const VmaAllocator & _allocator,
        const vk::DeviceSize bufferSize,
        const vk::BufferUsageFlags usage,
        const VmaMemoryUsage memoryUsageFlags
    ) :
        size(bufferSize)
    {
        vk::BufferCreateInfo bufferInfo = {};
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = vk::SharingMode::eExclusive;

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = memoryUsageFlags;

        auto vanillaBufferInfo = (VkBufferCreateInfo)bufferInfo;

        vmaCreateBuffer(_allocator, &vanillaBufferInfo, &allocInfo, &buffer, &allocation, nullptr);
    }

    Buffer::Buffer(Buffer && otherBuffer)
    {
        buffer = otherBuffer.buffer;
        allocation = otherBuffer.allocation;
        size = otherBuffer.size;

        otherBuffer.buffer = VK_NULL_HANDLE;
        otherBuffer.allocation = VmaAllocation();
        otherBuffer.size = 0;
    }

    Buffer & Buffer::operator=(Buffer && otherBuffer)
    {
        if (this != &otherBuffer) {
            buffer = otherBuffer.buffer;
            allocation = otherBuffer.allocation;
            size = otherBuffer.size;

            otherBuffer.buffer = VK_NULL_HANDLE;
            otherBuffer.allocation = VmaAllocation();
            otherBuffer.size = 0;
        }
        return *this;
    }

    Buffer::operator vk::Buffer() const
    {
        return vk::Buffer(buffer);
    }

    Buffer::operator VkBuffer() const
    {
        return buffer;
    }

    void Buffer::copyInto(const VmaAllocator & _allocator, const void* dataSrc)
    {
        void* mappedData;
        vmaMapMemory(_allocator, allocation, &mappedData);
        memcpy(mappedData, dataSrc, (size_t)size);
        vmaUnmapMemory(_allocator, allocation);
    }

    void Buffer::copyInto(const VmaAllocator & _allocator, const void* dataSrc, const size_t newSize)
    {
        void* mappedData;

        vmaMapMemory(_allocator, allocation, &mappedData);
        memcpy(mappedData, dataSrc, newSize);
        vmaUnmapMemory(_allocator, allocation);
    }

    void Buffer::destroy(const VmaAllocator & _allocator)
    {
        vmaDestroyBuffer(_allocator, buffer, allocation);
    }
}