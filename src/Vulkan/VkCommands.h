#ifndef PONG_VKAPP_COMMANDS_H
#define PONG_VKAPP_COMMANDS_H

#include <vulkan/vulkan.h>
#include <vector>

namespace Pong::VkApp
{
    struct SimpleCmdBufferAllocator
    {
        struct RecordScope
        {
            RecordScope(VkCommandBufferBeginInfo, VkCommandBuffer);

            auto getRecordedCmdBuffer() const { return _cmdBuffer; }

            ~RecordScope();

            private:
                VkCommandBuffer _cmdBuffer;
        };

        SimpleCmdBufferAllocator(uint32_t queueFamilyIndex, uint32_t cmdBuffersCount, VkCommandPoolCreateFlags flags = 0, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

        RecordScope RecordBuffer(size_t i, VkCommandBufferUsageFlags usage);

        void ResetCommandBuffers() const;
        void Delete() const;

        private:
            uint32_t _queueFamilyIndex; 
            VkCommandPool _cmdPool;
            std::vector<VkCommandBuffer> _cmdBuffers;
    };

}

#endif