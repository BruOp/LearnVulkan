#pragma once
#include <vulkan/vulkan.hpp>
#include <unordered_map>

#include "ImageLoader.h"
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
            const vkr::CommandManager & commandManager
        );

        Image(const Image&) = delete;
        Image& operator=(const Image&) = delete;

        Image(Image&& otherImage) = default;
        Image& operator=(Image&& otherImage) = default;

        void destroy(const VmaAllocator & allocator);

        void transitionImageLayout(
            const vkr::CommandManager & commandManager,
            vk::ImageLayout oldLayout,
            vk::ImageLayout newLayout
        );

        inline vk::Image get() const { return _image; };
        inline vk::Format getFormat() const { return _format; }

        static const std::unordered_map<vk::ImageLayout, vk::AccessFlagBits> layoutToAccessMaskMap;
        static const std::unordered_map<vk::ImageLayout, vk::PipelineStageFlagBits> layoutToStageMap;

    protected:
        uint32_t _width, _height;
        vk::Format _format;
        vk::Image _image;
        VmaAllocation _allocation;

    private:
        void copyBufferToImage(
            vkr::Buffer & buffer,
            const vkr::CommandManager & commandManager
        );
    };
}