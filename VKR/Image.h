#pragma once
#include <vulkan/vulkan.hpp>
#include <stb_image.h>
#include <unordered_map>

#include "Buffer.h"
#include "CommandManager.h"

namespace vkr
{
    class Image
    {
    public:
        Image();
        Image(
            const std::string & filePath,
            const VmaAllocator & allocator,
            vkr::CommandManager & commandManager
        );

        Image(const Image&) = delete;
        Image& operator=(const Image&) = delete;

        Image(Image&& otherImage);
        Image& operator=(Image&& otherImage);

        void destroy(const VmaAllocator & allocator);

        void transitionImageLayout(
            vkr::CommandManager & commandManager,
            vk::Format format,
            vk::ImageLayout oldLayout,
            vk::ImageLayout newLayout
        );

        static const std::unordered_map<vk::ImageLayout, vk::AccessFlagBits> layoutToAccessMaskMap;
        static const std::unordered_map<vk::ImageLayout, vk::PipelineStageFlagBits> layoutToStageMap;

    private:
        uint32_t _width, _height;
        VkImage _image;
        VmaAllocation _allocation;

        void copyBufferToImage(
            vkr::Buffer & buffer,
            vkr::CommandManager & commandManager
        );
    };
}