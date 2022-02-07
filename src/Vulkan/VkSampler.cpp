#include "VkSampler.h"
#include "VkMain.h"
#include "ErrorLog.h"

Pong::VkApp::Sampler::Sampler(VkFilter minMagFilter) 
{
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.maxLod = .0f;
    samplerInfo.minLod = .0f;
    samplerInfo.mipLodBias = 0.f;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    samplerInfo.anisotropyEnable = VK_FALSE;
    samplerInfo.maxAnisotropy = 1.f;
    samplerInfo.minFilter = minMagFilter;
    samplerInfo.magFilter = minMagFilter;

    if(vkCreateSampler(VkCore::Device(), &samplerInfo, nullptr, &_sampler) != VK_SUCCESS)    
    {
        LOG("failed to create sampler");
    }
}

void Pong::VkApp::Sampler::Delete() const
{
    vkDestroySampler(VkCore::Device(), _sampler, nullptr);    
}
