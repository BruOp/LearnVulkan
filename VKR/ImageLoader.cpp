#define STB_IMAGE_IMPLEMENTATION
#include "ImageLoader.h"


namespace vkr
{
    ImageLoader::ImageInfo ImageLoader::loadFromFile(const std::string & filePath, const vk::Format format)
    {
        int texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load(filePath.c_str(), &texWidth, &texHeight, &texChannels, formatMap.at(format));

        if (!pixels) {
            throw std::runtime_error("Failed to load texture image!");
        }
        ImageLoader::ImageInfo imageInfo{ (uint32_t)texWidth, (uint32_t)texHeight, (uint32_t)texChannels, format, pixels };
        return imageInfo;
    }

    void ImageLoader::freeData(ImageInfo & imageInfo)
    {
        stbi_image_free(imageInfo.data);
        imageInfo.data = nullptr;
    }

    vk::DeviceSize ImageLoader::getSize(const ImageInfo & imageInfo)
    {
        vk::DeviceSize deviceSize{ imageInfo.width * imageInfo.height * multiplierMap.at(imageInfo.format) };
        return deviceSize;
    }
}