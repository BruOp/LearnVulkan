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
            const std::string & filePath,
            const VmaAllocator & allocator,
            const vkr::CommandManager & commandManager
        );

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        Texture(Texture&& otherTexture);
        Texture& operator=(Texture&& otherTexture);

    private:
        vkr::Image _image;
        vkr::ImageView _imageView;
        vk::Format _format = vk::Format::eR8G8B8A8Unorm;
    };
}
