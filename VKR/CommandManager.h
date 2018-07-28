#pragma once
#include <vulkan/vulkan.hpp>

#include "QueueFamilyChecker.h"
#include "CommandBufferUtils.h"

namespace vkr
{
    class CommandManager
    {
    public:
        CommandManager();
        CommandManager(
            vk::Device device,
            const vkr::QueueFamilyChecker checker
        );
        ~CommandManager();

        CommandManager(const CommandManager&) = delete;
        CommandManager& operator=(const CommandManager&) = delete;

        CommandManager(CommandManager&& otherCommandManager);
        CommandManager& operator=(CommandManager&& otherCommandManager);

        vk::Device _device;
        vk::CommandPool _commandPool;

        vk::Queue _graphicsQueue;
        vk::Queue _presentQueue;

        void destroy();

        void freeCommandBuffers(const std::vector<vk::CommandBuffer> & commandBuffers);

        vk::CommandBuffer createOneTimeCommand();
        void endCommandBuffer(vk::CommandBuffer & commandBuffer);

    };
}

