#ifndef PONG_VKAPP_SAMPLER_H
#define PONG_VKAPP_SAMPLER_H

#include <vulkan/vulkan.h>

namespace Pong::VkApp
{
    struct Sampler
    {
        Sampler(VkFilter minMagFilter);
        void Delete() const;

        operator VkSampler() const 
        {
            return _sampler;
        }

        private:    
            VkSampler _sampler;
    };
}

#endif