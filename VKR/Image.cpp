#include "Image.h"


namespace vkr
{
    Image::Image() : _image(VK_NULL_HANDLE), _allocation()
    {
    }

    Image::Image(
        const std::string & filePath,
        const VmaAllocator & allocator,
        vkr::CommandManager & commandManager
    ) : Image()
    {
        auto format = vk::Format::eR8G8B8A8Unorm;
        vkr::ImageLoader::ImageInfo imageInfo{ vkr::ImageLoader::loadFromFile(filePath, format) };

        _width = imageInfo.width;
        _height = imageInfo.height;
        vk::DeviceSize imageSize{ vkr::ImageLoader::getSize(imageInfo) }; // The pixels are laid out row by row with 4 bytes per pixel in the case of STBI_rgba_alpha

        vkr::Buffer stagingBuffer{ allocator, imageSize, vk::BufferUsageFlagBits::eTransferSrc, VMA_MEMORY_USAGE_CPU_ONLY };
        stagingBuffer.copyInto(allocator, imageInfo.data);

        vkr::ImageLoader::freeData(imageInfo);

        auto initialLayout = vk::ImageLayout::eUndefined;
        auto copyLayout = vk::ImageLayout::eTransferDstOptimal;
        auto finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

        vk::ImageCreateInfo imageCreateInfo{};
        imageCreateInfo.imageType = vk::ImageType::e2D;
        imageCreateInfo.format = format;
        imageCreateInfo.extent = vk::Extent3D{ _width, _height, 1 };
        imageCreateInfo.tiling = vk::ImageTiling::eOptimal;
        imageCreateInfo.mipLevels = 1;
        imageCreateInfo.arrayLayers = 1;
        imageCreateInfo.initialLayout = initialLayout;

        imageCreateInfo.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
        imageCreateInfo.sharingMode = vk::SharingMode::eExclusive; // Only used by one queue family at a time
        imageCreateInfo.samples = vk::SampleCountFlagBits::e1;

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        VkImageCreateInfo vanillaInfo = (VkImageCreateInfo)imageCreateInfo;
        vmaCreateImage(allocator, &vanillaInfo, &allocInfo, &_image, &_allocation, nullptr);

        transitionImageLayout(commandManager, format, initialLayout, copyLayout);

        copyBufferToImage(stagingBuffer, commandManager);

        transitionImageLayout(commandManager, format, copyLayout, finalLayout);

        stagingBuffer.destroy(allocator);
    }

    Image::Image(Image && otherImage)
    {
        _image = otherImage._image;
        _allocation = otherImage._allocation;

        otherImage._image = VK_NULL_HANDLE;
        otherImage._allocation = VmaAllocation();
    }

    Image & Image::operator=(Image && otherImage)
    {
        if (this != &otherImage) {
            _image = otherImage._image;
            _allocation = otherImage._allocation;

            otherImage._image = VK_NULL_HANDLE;
            otherImage._allocation = VmaAllocation();
        }

        return *this;
    }

    void Image::destroy(const VmaAllocator & allocator)
    {
        vmaDestroyImage(allocator, _image, _allocation);
    }

    void Image::transitionImageLayout(
        vkr::CommandManager & commandManager,
        vk::Format format,
        vk::ImageLayout oldLayout,
        vk::ImageLayout newLayout
    )
    {
        vk::CommandBuffer command{ commandManager.createOneTimeCommand() };

        vk::ImageMemoryBarrier barrier{};
        barrier.setOldLayout(oldLayout);
        barrier.setNewLayout(newLayout);
        barrier.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
        barrier.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
        barrier.setImage(_image);
        // Use the map to get the accessMasks
        barrier.setSrcAccessMask(layoutToAccessMaskMap.at(oldLayout));
        barrier.setDstAccessMask(layoutToAccessMaskMap.at(newLayout));

        vk::ImageSubresourceRange subresourceRange{ vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 };

        barrier.setSubresourceRange(subresourceRange);

        command.pipelineBarrier(
            layoutToStageMap.at(oldLayout),
            layoutToStageMap.at(newLayout),
            (vk::DependencyFlagBits)0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );

        commandManager.endCommandBuffer(command);
    }

    void Image::copyBufferToImage(vkr::Buffer & buffer, vkr::CommandManager & commandManager)
    {
        vk::CommandBuffer command{ commandManager.createOneTimeCommand() };
        vk::ImageSubresourceLayers layers{ vk::ImageAspectFlagBits::eColor, 0U, 0U, 1U };
        vk::BufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0; // These two fields actually set padding, not actual lengths FYI
        region.bufferImageHeight = 0;
        region.imageSubresource = layers;
        region.imageOffset = vk::Offset3D{};
        region.imageExtent = vk::Extent3D{ _width, _height, 1 };

        command.copyBufferToImage(buffer, _image, vk::ImageLayout::eTransferDstOptimal, { region });

        commandManager.endCommandBuffer(command);
    }

    const std::unordered_map<vk::ImageLayout, vk::AccessFlagBits> Image::layoutToAccessMaskMap{
        { vk::ImageLayout::eUndefined, (vk::AccessFlagBits)0 },
        { vk::ImageLayout::eTransferDstOptimal, vk::AccessFlagBits::eTransferWrite },
        { vk::ImageLayout::eShaderReadOnlyOptimal, vk::AccessFlagBits::eShaderRead },
    };

    const std::unordered_map<vk::ImageLayout, vk::PipelineStageFlagBits> Image::layoutToStageMap{
        { vk::ImageLayout::eUndefined, vk::PipelineStageFlagBits::eTopOfPipe },
        { vk::ImageLayout::eTransferDstOptimal, vk::PipelineStageFlagBits::eTransfer },
        { vk::ImageLayout::eShaderReadOnlyOptimal, vk::PipelineStageFlagBits::eFragmentShader },
    };
}