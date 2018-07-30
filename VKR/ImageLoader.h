#pragma once
#include <vulkan/vulkan.hpp>
#include <stb_image.h>
#include <unordered_map>
#include <tuple>


namespace vkr
{
    class ImageLoader
    {
    public:

        ImageLoader(const std::string & filePath, const vk::Format format);
        ~ImageLoader();

        vk::DeviceSize getSize();

        uint32_t width;
        uint32_t height;
        uint32_t channels;
        vk::Format format;
        stbi_uc * data;

    private:
        const std::unordered_map<vk::Format, int> formatMap = {
            { vk::Format::eR8G8B8A8Unorm, STBI_rgb_alpha }
        };
        const std::unordered_map<vk::Format, int> multiplierMap = {
            { vk::Format::eR8G8B8A8Unorm, 4 }
        };
    };
}
