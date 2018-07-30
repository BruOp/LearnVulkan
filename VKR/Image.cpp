#include "Image.h"


namespace vkr
{
    Image::Image() :
        _width(0),
        _height(0),
        _image(vk::Image{}),
        _allocation(),
        _format(vk::Format::eR8G8B8A8Unorm)
    {
    }

    Image::Image(
        const std::string & filePath,
        const VmaAllocator & allocator,
        const vkr::CommandManager & commandManager
    ) : Image()
    {
        vkr::ImageLoader imageLoader{ filePath, _format };

        _width = imageLoader.width;
        _height = imageLoader.height;
        vk::DeviceSize imageSize{ imageLoader.getSize() }; // The pixels are laid out row by row with 4 bytes per pixel in the case of STBI_rgba_alpha

        vkr::Buffer stagingBuffer{ allocator, imageSize, vk::BufferUsageFlagBits::eTransferSrc, VMA_MEMORY_USAGE_CPU_ONLY };
        stagingBuffer.copyInto(allocator, imageLoader.data);

        auto initialLayout = vk::ImageLayout::eUndefined;
        auto copyLayout = vk::ImageLayout::eTransferDstOptimal;
        auto finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

        vk::ImageCreateInfo imageCreateInfo{};
        imageCreateInfo.imageType = vk::ImageType::e2D;
        imageCreateInfo.format = _format;
        imageCreateInfo.extent = vk::Extent3D{ _width, _height, 1 };
        imageCreateInfo.tiling = vk::ImageTiling::eOptimal;
        imageCreateInfo.mipLevels = 1;
        imageCreateInfo.arrayLayers = 1;
        imageCreateInfo.initialLayout = initialLayout;

        imageCreateInfo.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
        imageCreateInfo.sharingMode = vk::SharingMode::eExclusive; // Only used by one queue family at a time
        imageCreateInfo.samples = vk::SampleCountFlagBits::e1;

        VkImage image;
        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        VkImageCreateInfo vanillaInfo = (VkImageCreateInfo)imageCreateInfo;

        vmaCreateImage(allocator, &vanillaInfo, &allocInfo, &image, &_allocation, nullptr);
        _image = image;

        transitionImageLayout(commandManager, initialLayout, copyLayout);

        copyBufferToImage(stagingBuffer, commandManager);

        transitionImageLayout(commandManager, copyLayout, finalLayout);

        stagingBuffer.destroy(allocator);
    }

    Image::Image(Image && otherImage)
    {
        _image = otherImage._image;
        _allocation = otherImage._allocation;

        otherImage._image = vk::Image{};
        otherImage._allocation = VmaAllocation();
    }

    Image & Image::operator=(Image && otherImage)
    {
        if (this != &otherImage) {
            _image = otherImage._image;
            _allocation = otherImage._allocation;

            otherImage._image = vk::Image{};
            otherImage._allocation = VmaAllocation();
        }

        return *this;
    }

    void Image::destroy(const VmaAllocator & allocator)
    {
        vmaDestroyImage(allocator, _image, _allocation);
    }

    void Image::transitionImageLayout(
        const vkr::CommandManager & commandManager,
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

    void Image::copyBufferToImage(vkr::Buffer & buffer, const vkr::CommandManager & commandManager)
    {
        vk::CommandBuffer command{ commandManager.createOneTimeCommand() };
        vk::ImageSubresourceLayers layers{
            vk::ImageAspectFlagBits::eColor, //Aspect mask
            0U, // mip level
            0U, // base array layer
            1U  // layer count
        };
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