#ifndef PONG_VKAPP_FRAMEBUFFER_H
#define PONG_VKAPP_FRAMEBUFFER_H

#include <vulkan/vulkan.h>

namespace Pong::VkApp
{
    struct Framebuffer
    {   
        Framebuffer(VkRenderPass renderpass, int32_t vpW, int32_t vpH, VkFormat imgFormat, VkImage img, VkImageAspectFlags imgAspects);
       
        auto get() const { return _fbo; }

        void Resize(VkRenderPass renderpass, VkImage img, int32_t vpW, int32_t vpH);
        void Delete() const;

        private:
            VkImageViewCreateInfo _currentFboImgViewInfo{};
            VkFramebufferCreateInfo _currentFboInfo{};

            VkImageView _fboImgView;
            VkFramebuffer _fbo;
    };
}

#endif
