#include "VkFramebuffer.h"
#include "VkMain.h"
#include "ErrorLog.h"


Pong::VkApp::Framebuffer::Framebuffer(VkRenderPass renderpass, int32_t vpW, int32_t vpH, VkFormat imgFormat, VkImage img, VkImageAspectFlags imgAspects)
{
    _currentFboImgViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    _currentFboImgViewInfo.pNext = nullptr;
    _currentFboImgViewInfo.flags = 0;
    _currentFboImgViewInfo.components = {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY};
    _currentFboImgViewInfo.format = imgFormat;
    _currentFboImgViewInfo.image = img;
    _currentFboImgViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    _currentFboImgViewInfo.subresourceRange.aspectMask = imgAspects;
    _currentFboImgViewInfo.subresourceRange.baseArrayLayer = 0;
    _currentFboImgViewInfo.subresourceRange.layerCount = 1;
    _currentFboImgViewInfo.subresourceRange.baseMipLevel = 0;
    _currentFboImgViewInfo.subresourceRange.levelCount = 1;

    if(vkCreateImageView(VkCore::Device(), &_currentFboImgViewInfo, nullptr, &_fboImgView) != VK_SUCCESS)
    {
        LOG("failed to create img view");
    }


    _currentFboInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    _currentFboInfo.pNext = nullptr;
    _currentFboInfo.flags = 0;
    _currentFboInfo.width = vpW;
    _currentFboInfo.height = vpH;
    _currentFboInfo.renderPass = renderpass;
    _currentFboInfo.attachmentCount = 1;
    _currentFboInfo.pAttachments = &_fboImgView;
    _currentFboInfo.layers = 1;

    if(vkCreateFramebuffer(VkCore::Device(), &_currentFboInfo, nullptr, &_fbo) != VK_SUCCESS)
    {
        LOG("failed to create fbo");
    }
}

void Pong::VkApp::Framebuffer::Resize(VkRenderPass renderpass, VkImage img, int32_t vpW, int32_t vpH)
{
    Delete();

    _currentFboImgViewInfo.image = img;

    _currentFboInfo.renderPass = renderpass;
    _currentFboInfo.width = vpW;
    _currentFboInfo.height = vpH;    

    if(vkCreateImageView(VkCore::Device(), &_currentFboImgViewInfo, nullptr, &_fboImgView) != VK_SUCCESS)
    {
        LOG("failed to resize img view");
        return;
    }

    if(vkCreateFramebuffer(VkCore::Device(), &_currentFboInfo, nullptr, &_fbo) != VK_SUCCESS)
    {
        LOG("failed to resize fbo");
        return;
    }
}

void Pong::VkApp::Framebuffer::Delete() const
{
    vkDestroyImageView(VkCore::Device(), _fboImgView, nullptr);
    vkDestroyFramebuffer(VkCore::Device(), _fbo, nullptr);
}
