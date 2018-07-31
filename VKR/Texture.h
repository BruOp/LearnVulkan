#pragma once

#include "Image.h"
#include "ImageView.h"

namespace vkr
{
    class Texture
    {
    public:
        Texture();
        Texture(
            const vk::Device & device,
            const VmaAllocator & allocator,
            const vkr::CommandManager & commandManager,
            const std::string & filePath
        );

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        Texture(Texture&& otherTexture) = default;
        Texture& operator=(Texture&& otherTexture) = default;

        inline void destroy(const vk::Device & device, VmaAllocator & allocator) {
            _image.destroy(allocator);
            _imageView.destroy(device);
            device.destroySampler(_sampler);
        };

        vk::DescriptorImageInfo getImageInfo() const;

    private:
        vkr::Image _image;
        vkr::ImageView _imageView;
        vk::Sampler _sampler;
        vk::Format _format = vk::Format::eR8G8B8A8Unorm;
    };
}
