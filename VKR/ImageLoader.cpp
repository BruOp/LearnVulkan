#define STB_IMAGE_IMPLEMENTATION
#include "ImageLoader.h"


namespace vkr
{
    ImageLoader::ImageLoader(const std::string & filePath, const vk::Format format) :
        width(0),
        height(0),
        channels(0),
        format(format),
        data(nullptr)
    {
        int texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load(filePath.c_str(), &texWidth, &texHeight, &texChannels, formatMap.at(format));

        if (!pixels) {
            throw std::runtime_error("Failed to load texture image!");
        }

        width = (uint32_t)texWidth;
        height = (uint32_t)texHeight;
        channels = (uint32_t)texChannels;
        data = pixels;

    }

    ImageLoader::~ImageLoader()
    {
        stbi_image_free(data);
    }

    vk::DeviceSize ImageLoader::getSize()
    {
        vk::DeviceSize deviceSize{ width * height * multiplierMap.at(format) };
        return deviceSize;
    }
}