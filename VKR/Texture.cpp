#include "Texture.h"


namespace vkr
{
    Texture::Texture()
    {
    }

    Texture::Texture(
        const vk::Device & device,
        const VmaAllocator & allocator,
        const vkr::CommandManager & commandManager,
        const std::string & filePath
    ) :
        _image(filePath, allocator, commandManager),
        _format(_image.getFormat()),
        _imageView(device, _image)
    {
        vk::SamplerCreateInfo samplerInfo;
        samplerInfo.magFilter = vk::Filter::eLinear;
        samplerInfo.minFilter = vk::Filter::eLinear; // Eventually we'll want mip maps for this.
        samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
        samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
        samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = 16;

        samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
        samplerInfo.unnormalizedCoordinates = VK_FALSE; // Want uv to both be [0, 1)
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = vk::CompareOp::eAlways;

        samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;

        _sampler = device.createSampler(samplerInfo);
    }

    vk::DescriptorImageInfo Texture::getImageInfo() const
    {
        vk::DescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        imageInfo.imageView = _imageView;
        imageInfo.sampler = _sampler;
        return imageInfo;
    }
}
