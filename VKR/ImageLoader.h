#pragma once
#include <vulkan/vulkan.hpp>
#include <stb_image.h>
#include <unordered_map>
#include <tuple>


namespace vkr
{
    namespace ImageLoader
    {
        struct ImageInfo
        {
            uint32_t width;
            uint32_t height;
            uint32_t channels;
            vk::Format format;
            stbi_uc * data;
        };

        ImageInfo loadFromFile(const std::string & filePath, const vk::Format);

        void freeData(ImageInfo & imageInfo);

        int getSTBFormat(const vk::Format);
        vk::DeviceSize getSize(const ImageInfo & imageInfo);

        const std::unordered_map<vk::Format, int> formatMap = {
            { vk::Format::eR8G8B8A8Unorm, STBI_rgb_alpha }
        };
        const std::unordered_map<vk::Format, int> multiplierMap = {
            { vk::Format::eR8G8B8A8Unorm, 4 }
        };
    };
}
