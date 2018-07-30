#pragma once
#include <vulkan/vulkan.hpp>
#include "Image.h"

namespace vkr
{
    class ImageView
    {
        friend class vkr::Image;

    public:
        ImageView();
        ImageView(const vk::Device & device, const vkr::Image & image);
        ImageView(const vk::Device & device, const vk::Image image, const vk::Format format);

        inline operator vk::ImageView() const { return _imageView; };

        inline void destroy(const vk::Device & device)
        {
            device.destroyImageView(_imageView);
        }

    private:
        vk::ImageView _imageView;
    };
}