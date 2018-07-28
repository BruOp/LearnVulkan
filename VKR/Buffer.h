#pragma once
#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>


namespace vkr
{
    class Buffer
    {
    public:
        Buffer();
        Buffer(
        const VmaAllocator & _allocator,
            const vk::DeviceSize bufferSize,
            const vk::BufferUsageFlags usage,
            VmaMemoryUsage memoryUsageFlags
        );
        // No copying allowed!
        Buffer(const Buffer&) = delete;
        Buffer& operator=(const Buffer&) = delete;

        Buffer(Buffer&& otherBuffer);
        Buffer& operator=(Buffer&& otherBuffer);

        operator vk::Buffer() const;
        operator VkBuffer() const;

        void destroy(const VmaAllocator & _allocator);

        inline VkBuffer get() const { return buffer; };

        void copyInto(const VmaAllocator & _allocator, const void* dataSrc);
        void copyInto(const VmaAllocator & _allocator, const void* dataSrc, const size_t newSize);

        vk::DeviceSize size;

        VkBuffer buffer;
        VmaAllocation allocation;
    private:
    };
}