#include "ImageView.h"

namespace vkr
{
    ImageView::ImageView()
    {
    }

    ImageView::ImageView(const vk::Device & device, const vk::Image image, const vk::Format format)
    {
        vk::ImageViewCreateInfo createInfo = {};
        createInfo.image = image;
        createInfo.format = format;
        createInfo.viewType = vk::ImageViewType::e2D;

        createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        _imageView = device.createImageView(createInfo);
    }

    ImageView::ImageView(const vk::Device & device, const vkr::Image & image) :
        ImageView(device, image.get(), image.getFormat())
    {
    }


}