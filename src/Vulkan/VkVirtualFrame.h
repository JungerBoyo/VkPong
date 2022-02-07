#ifndef PONG_VKAPP_VIRTUALFRAME_H
#define PONG_VKAPP_VIRTUALFRAME_H

#include <vulkan/vulkan.h>

#include "VkFramebuffer.h"
#include "VkCommands.h"

namespace Pong::VkApp
{
    struct VirtualFrame
    {
        VirtualFrame(VkRenderPass renderpass, int32_t vpW, int32_t vpH, VkFormat imgFormat, VkImage img, VkImageAspectFlags imgAspects);
        void Delete() const;

        Framebuffer fbo;
        SimpleCmdBufferAllocator cmdAlloc;
        VkSemaphore imgAquiredSemaphore;
        VkSemaphore renderFinishedSemaphore;
        VkFence cmdBufferSubmissionFence;
    };
} 


#endif