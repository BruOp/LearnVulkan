#include "BufferUtils.h"


namespace vkr
{
    namespace BufferUtils
    {
        void copyBuffer(
            vkr::CommandManager & commandManager,
            vkr::Buffer & stagingBuffer,
            vkr::Buffer & gpuBuffer
        )
        {
            vk::CommandBuffer commandBuffer { commandManager.createOneTimeCommand() };

            vk::BufferCopy copyRegion = {};
            copyRegion.srcOffset = 0;
            copyRegion.dstOffset = 0;
            copyRegion.size = stagingBuffer.size;

            commandBuffer.copyBuffer(stagingBuffer.buffer, gpuBuffer.buffer, 1, &copyRegion);

            commandManager.endCommandBuffer(commandBuffer);
        }
    }
}
