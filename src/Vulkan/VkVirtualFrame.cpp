#include "VkVirtualFrame.h"
#include "ErrorLog.h"
#include "VkMain.h"

Pong::VkApp::VirtualFrame::VirtualFrame(VkRenderPass renderpass, int32_t vpW, int32_t vpH, VkFormat imgFormat, VkImage img, VkImageAspectFlags imgAspects)
 : fbo(renderpass, vpW, vpH, imgFormat, img, imgAspects),
   cmdAlloc(VkCore::GraphicsQueueIndex(), 1, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT|VK_COMMAND_POOL_CREATE_TRANSIENT_BIT)    
{
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if(vkCreateFence(VkCore::Device(), &fenceInfo, nullptr, &cmdBufferSubmissionFence) != VK_SUCCESS)
    {
        LOG("failed to create fence");
    }

    VkSemaphoreCreateInfo semaphoreInfo{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    if(vkCreateSemaphore(VkCore::Device(), &semaphoreInfo, nullptr, &imgAquiredSemaphore) != VK_SUCCESS ||
       vkCreateSemaphore(VkCore::Device(), &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS)
    {
        LOG("failed to create semaphores");
    }
}

void Pong::VkApp::VirtualFrame::Delete() const 
{
    cmdAlloc.Delete();
    fbo.Delete();
    vkDestroyFence(VkCore::Device(), cmdBufferSubmissionFence, nullptr);
    vkDestroySemaphore(VkCore::Device(), imgAquiredSemaphore, nullptr);
    vkDestroySemaphore(VkCore::Device(), renderFinishedSemaphore, nullptr);    
}
