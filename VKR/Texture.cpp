#include "Texture.h"


namespace vkr
{
    Texture::Texture()
    {
    }

    Texture::Texture(const std::string & filePath, const VmaAllocator & allocator, const vkr::CommandManager & commandManager)
    {

    }

    Texture::Texture(Texture && otherTexture)
    {
        _image = std::move(otherTexture._image);
        _imageView = otherTexture._imageView;
        _format = otherTexture._format;

        otherTexture._image = vkr::Image{};
        otherTexture._imageView = vkr::ImageView{};
        otherTexture._format = vk::Format{};
    }

    Texture & Texture::operator=(Texture && otherTexture)
    {
        if (this != &otherTexture) {
            _image = std::move(otherTexture._image);
            _imageView = otherTexture._imageView;
            _format = otherTexture._format;

            otherTexture._image = vkr::Image{};
            otherTexture._imageView = vkr::ImageView{};
            otherTexture._format = vk::Format{};
        }
        return *this;
    }
}
