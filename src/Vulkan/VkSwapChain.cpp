#include "VkSwapChain.h"
#include "VkMain.h"
#include "ErrorLog.h"


Pong::VkApp::SwapChainDetailes Pong::VkApp::QuerySwapChainDetailes(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &capabilities);

    uint32_t formatCount{0};
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

    std::vector<VkSurfaceFormatKHR> formats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, formats.data());


    uint32_t presModeCount{0};
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presModeCount, nullptr);
    
    std::vector<VkPresentModeKHR> presModes(presModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presModeCount, presModes.data());



    uint32_t imgCount{capabilities.minImageCount + 1};
    if(capabilities.maxImageCount && imgCount > capabilities.maxImageCount)
        imgCount = capabilities.maxImageCount;


    return {capabilities, formats, presModes, imgCount};
}   

Pong::VkApp::Swapchain::Swapchain(int32_t vpW, int32_t vpH, const SwapChainDetailes& details)
{
    bool formatAvailable{false};
    for(const auto& format : details.formats)
        if(format.colorSpace == surfaceFormat.colorSpace &&
           format.format == surfaceFormat.format)
            formatAvailable = true;

    if(!formatAvailable)
    {
        LOG("chosen swapchain image format not available, swapchain wont be created");
        return;
    }

    if(!(details.capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT))
    {
        LOG("transfer operation not supported, swapchain wont be created");
        return;
    }

    _minSchExtent = details.capabilities.minImageExtent;
    _maxSchExtent = details.capabilities.maxImageExtent;

    if(details.capabilities.currentExtent.width == -1)
    {
        _swapchainExtent.width = std::clamp(static_cast<uint32_t>(vpW), _minSchExtent.width, _maxSchExtent.width);
        _swapchainExtent.height = std::clamp(static_cast<uint32_t>(vpH), _minSchExtent.height, _maxSchExtent.height);
    }
    else
        _swapchainExtent = details.capabilities.currentExtent;


    if(details.capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
        _transformations = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR; 
    else 
        _transformations = details.capabilities.currentTransform;
    
   
    bool mailbox{false};
    for(auto presentMode : details.presentationModes)
        if(presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            mailbox = true;
            _presentationMode = VK_PRESENT_MODE_MAILBOX_KHR;
        }
            
    if(!mailbox)
        _presentationMode = VK_PRESENT_MODE_FIFO_KHR;


    VkSwapchainCreateInfoKHR swapchainInfo{};
    swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainInfo.imageExtent = _swapchainExtent;
    swapchainInfo.imageArrayLayers = 1;
    swapchainInfo.flags = 0;
    swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainInfo.clipped = VK_TRUE;
    swapchainInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapchainInfo.imageFormat = surfaceFormat.format;
    swapchainInfo.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT|VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainInfo.minImageCount = details.imageCount;
    swapchainInfo.oldSwapchain = VK_NULL_HANDLE;
    swapchainInfo.pNext = nullptr;
    swapchainInfo.presentMode = _presentationMode;
    swapchainInfo.preTransform = _transformations;
    swapchainInfo.surface = VkCore::Surface();
    swapchainInfo.queueFamilyIndexCount = 0;
    swapchainInfo.pQueueFamilyIndices = nullptr;
    swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
 
    if(vkCreateSwapchainKHR(VkCore::Device(), &swapchainInfo, nullptr, &_swapchain) != VK_SUCCESS)
    {
        LOG("failed to create swapchain");
        return;
    }    

    uint32_t schImgCount;
    vkGetSwapchainImagesKHR(VkCore::Device(), _swapchain, &schImgCount, nullptr);
    if(!schImgCount)
    {
        LOG("swapchain is not set properly, image count = 0");
        return;
    }

    _schImages.resize(schImgCount);
    vkGetSwapchainImagesKHR(VkCore::Device(), _swapchain, &schImgCount, _schImages.data());
}


void Pong::VkApp::Swapchain::Recreate(int32_t vpW, int32_t vpH) 
{
    auto oldSwapchain = _swapchain;

    _swapchainExtent.width = static_cast<uint32_t>(vpW);
    _swapchainExtent.height = static_cast<uint32_t>(vpH);
    
    VkSwapchainCreateInfoKHR swapchainInfo{};
    swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainInfo.imageExtent = _swapchainExtent;
    swapchainInfo.imageArrayLayers = 1;
    swapchainInfo.flags = 0;
    swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainInfo.clipped = VK_TRUE;
    swapchainInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapchainInfo.imageFormat = surfaceFormat.format;
    swapchainInfo.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT|VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainInfo.minImageCount = _schImages.size();
    swapchainInfo.oldSwapchain = _swapchain;
    swapchainInfo.pNext = nullptr;
    swapchainInfo.presentMode = _presentationMode;
    swapchainInfo.preTransform = _transformations;
    swapchainInfo.surface = VkCore::Surface();
    swapchainInfo.queueFamilyIndexCount = 0;
    swapchainInfo.pQueueFamilyIndices = nullptr;
    swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
 
    if(vkCreateSwapchainKHR(VkCore::Device(), &swapchainInfo, nullptr, &_swapchain) != VK_SUCCESS)
    {
        LOG("failed to create swapchain");
        return;
    }    
    vkDestroySwapchainKHR(VkCore::Device(), oldSwapchain, nullptr);

    uint32_t schImgCount = _schImages.size();
    vkGetSwapchainImagesKHR(VkCore::Device(), _swapchain, &schImgCount, _schImages.data());
}

void Pong::VkApp::Swapchain::Delete() 
{
    if(_swapchain != VK_NULL_HANDLE)
    {
        vkDestroySwapchainKHR(VkCore::Device(), _swapchain, nullptr);
    }
}
