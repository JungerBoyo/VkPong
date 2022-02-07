#ifndef PONG_VKAPP_VKTEXTURE2D_H
#define PONG_VKAPP_VKTEXTURE2D_H

#include <vulkan/vulkan.h>

#include "VkMemoryAllocator.h"
#include "Img.h"

namespace Pong::VkApp
{
    struct Texture2D
    {
        Texture2D(const Img& img, VkImageLayout layout, VkImageTiling tiling, VkFormat format, VkImageAspectFlags aspect); 
      
        VkDescriptorImageInfo Info() const;
        void Delete() const;

        private:
            VkImageLayout _layout;
            VkImageView _imgView;
            VkImage _img;
            StagingMemoryAllocator _memAllocator;
    };  
}

#endif