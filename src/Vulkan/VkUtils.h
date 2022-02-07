#ifndef PONG_VKAPP_UTILS_H
#define PONG_VKAPP_UTILS_H

#include <vulkan/vulkan.h>

#include <tuple>

namespace Pong::VkApp
{
    auto CreateViewportScissor(uint32_t vpW, uint32_t vpH)
    {
        VkViewport vp;
        vp.minDepth = .0f; 
        vp.maxDepth = 1.f;
        vp.x = 0; 
        vp.y = 0;
        vp.width = static_cast<float>(vpW);
        vp.height = static_cast<float>(vpH);
    
        VkRect2D scissor;
        scissor.offset = {0, 0};
        scissor.extent = {vpW, vpH};

        return std::make_tuple(vp, scissor);
    }

}

#endif