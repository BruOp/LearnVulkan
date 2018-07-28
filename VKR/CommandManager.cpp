#include "CommandManager.h"


namespace vkr
{
    CommandManager::CommandManager() :
        _device(),
        _commandPool(),
        _graphicsQueue(),
        _presentQueue()
    {
    }

    CommandManager::CommandManager(
        vk::Device device,
        const vkr::QueueFamilyChecker checker
    ) :
        _device(device)
    {

        vk::CommandPoolCreateInfo poolInfo = {};
        poolInfo.queueFamilyIndex = checker.graphicsFamily;
        poolInfo.flags = vk::CommandPoolCreateFlagBits(0);

        _commandPool = device.createCommandPool(poolInfo);

        _graphicsQueue = vk::Queue{ device.getQueue(checker.graphicsFamily, 0) };
        _presentQueue = vk::Queue{ device.getQueue(checker.presentFamily, 0) };
    }


    CommandManager::~CommandManager()
    {
    }

    CommandManager::CommandManager(CommandManager && otherCommandManager)
    {
        _device = otherCommandManager._device;
        _commandPool = otherCommandManager._commandPool;
        _graphicsQueue = otherCommandManager._graphicsQueue;
        _presentQueue = otherCommandManager._presentQueue;

        otherCommandManager._device = vk::Device{};
        otherCommandManager._commandPool = vk::CommandPool{};
        otherCommandManager._graphicsQueue = vk::Queue{};
        otherCommandManager._presentQueue = vk::Queue{};
    }

    CommandManager & CommandManager::operator=(CommandManager && otherCommandManager)
    {
        if (this != &otherCommandManager) {
            _device = otherCommandManager._device;
            _commandPool = otherCommandManager._commandPool;
            _graphicsQueue = otherCommandManager._graphicsQueue;
            _presentQueue = otherCommandManager._presentQueue;

            otherCommandManager._device = vk::Device{};
            otherCommandManager._commandPool = vk::CommandPool{};
            otherCommandManager._graphicsQueue = vk::Queue{};
            otherCommandManager._presentQueue = vk::Queue{};
        }
        return *this;
    }

    void CommandManager::destroy()
    {
        _device.destroyCommandPool(_commandPool);
    }

    void CommandManager::freeCommandBuffers(const std::vector<vk::CommandBuffer> & commandBuffers)
    {
        _device.freeCommandBuffers(
            _commandPool,
            static_cast<uint32_t>(commandBuffers.size()),
            commandBuffers.data()
        );
    }
    vk::CommandBuffer CommandManager::createOneTimeCommand()
    {
        return CommandBufferUtils::createOneTimeCommand(_device, _commandPool);

    }
    void CommandManager::endCommandBuffer(vk::CommandBuffer & commandBuffer)
    {
        CommandBufferUtils::endCommandBuffer(commandBuffer, _device, _commandPool, _graphicsQueue);
    }
}
