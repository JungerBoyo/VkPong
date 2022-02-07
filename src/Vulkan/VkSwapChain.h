#ifndef PONG_VKAPP_SWAPCHAIN_H
#define PONG_VKAPP_SWAPCHAIN_H

#include <vulkan/vulkan.h>
#include <vector>

#include "VkCommands.h"
#include "VkFramebuffer.h"

namespace Pong::VkApp
{
    struct SwapChainDetailes
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentationModes;
        uint32_t imageCount;
    };

    struct Swapchain
    {
        Swapchain(int32_t vpW, int32_t vpH, const SwapChainDetailes&);

        auto getExtent() const { return _swapchainExtent; }
        auto getImage(size_t i) const { return _schImages[i]; }
        auto get() const { return _swapchain; }
        
        void Recreate(int32_t vpW, int32_t vpH);
        void Delete();

        static constexpr VkSurfaceFormatKHR surfaceFormat {
            VK_FORMAT_B8G8R8A8_UNORM,
            VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
        };

        private:
            std::vector<VkImage> _schImages;

            VkPresentModeKHR _presentationMode;
            VkSurfaceTransformFlagBitsKHR _transformations;
            VkSwapchainKHR _swapchain{VK_NULL_HANDLE};

            VkExtent2D _swapchainExtent{0, 0};     
            VkExtent2D _minSchExtent{0, 0};
            VkExtent2D _maxSchExtent{0, 0};

    };

    SwapChainDetailes QuerySwapChainDetailes(VkPhysicalDevice, VkSurfaceKHR);
}   

#endif