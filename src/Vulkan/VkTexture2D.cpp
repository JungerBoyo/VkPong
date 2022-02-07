#include "VkTexture2D.h"
#include "VkMain.h"

#include "ErrorLog.h"

Pong::VkApp::Texture2D::Texture2D(const Img& img, VkImageLayout layout, VkImageTiling tiling, VkFormat format, VkImageAspectFlags aspect) 
   : _layout(layout)
{
    VkImageCreateInfo imgInfo{};
    imgInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imgInfo.arrayLayers = 1;
    imgInfo.extent = {static_cast<uint32_t>(img.width), static_cast<uint32_t>(img.height), 1};
    imgInfo.format = format;
    imgInfo.imageType = VK_IMAGE_TYPE_2D;
    imgInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imgInfo.mipLevels = 1;
    imgInfo.pQueueFamilyIndices = nullptr;
    imgInfo.queueFamilyIndexCount = 0;
    imgInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imgInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imgInfo.tiling = tiling;
    imgInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT|VK_IMAGE_USAGE_SAMPLED_BIT;
    
    if(vkCreateImage(VkCore::Device(), &imgInfo, nullptr, &_img) != VK_SUCCESS)
    {
        LOG("failed to create img");
        return;
    }

    VkMemoryRequirements imgMemRequirements;
    vkGetImageMemoryRequirements(VkCore::Device(), _img, &imgMemRequirements);
    _memAllocator.Allocate(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, imgMemRequirements);
   
    auto[mem, offset] = _memAllocator.AcquireMemory(img.width * img.height * img.channels);
    vkBindImageMemory(VkCore::Device(), _img, mem, offset);

    _memAllocator.UploadData(img.pixels.data(), offset, {imgInfo.extent.width, imgInfo.extent.height}, 4, aspect, _img, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, layout);

    VkImageViewCreateInfo imgViewInfo{};
    imgViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imgViewInfo.format = format;
    imgViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imgViewInfo.components = {VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_IDENTITY};
    imgViewInfo.image = _img;
    imgViewInfo.subresourceRange.aspectMask = aspect;
    imgViewInfo.subresourceRange.baseArrayLayer = 0;
    imgViewInfo.subresourceRange.layerCount = 1;
    imgViewInfo.subresourceRange.baseMipLevel = 0;
    imgViewInfo.subresourceRange.levelCount = 1;

    if(vkCreateImageView(VkCore::Device(), &imgViewInfo, nullptr, &_imgView) != VK_SUCCESS)
    {
        LOG("failed to create img view");
    }
}

void Pong::VkApp::Texture2D::Delete() const 
{
    _memAllocator.Delete();
    vkDestroyImageView(VkCore::Device(), _imgView, nullptr);
    vkDestroyImage(VkCore::Device(), _img, nullptr);
}

VkDescriptorImageInfo Pong::VkApp::Texture2D::Info() const 
{
    VkDescriptorImageInfo imgInfo{};
    imgInfo.imageLayout = _layout;
    imgInfo.imageView = _imgView;
    imgInfo.sampler = VK_NULL_HANDLE;

    return imgInfo;
}
