#ifndef PONG_VKAPP_VKMAIN_H
#define PONG_VKAPP_VKMAIN_H

#include "VkSwapChain.h"

#include <vulkan/vulkan.h>

#include <vector>
#include <array>
#include <cinttypes>
#include <optional>

namespace Pong::VkApp
{
    constexpr std::array<const char*, 1> VALIDATION_LAYERS
    {{
        "VK_LAYER_KHRONOS_validation"
    }};

    constexpr bool VALIDATION_LAYERS_STATE{false};

    struct QueueFamilies
    { 
        std::optional<uint32_t> graphics;
        std::optional<uint32_t> presentation;
        std::optional<uint32_t> transfer;

        operator bool()
        {
            return graphics.has_value()     &&
                   presentation.has_value() &&
                   transfer.has_value()       ; 
        }

    };

    struct VkCore
    {
        static void CreateInstance(const std::vector<const char*>& extensions);
        static void PickPhysicalDevice(const std::vector<const char*>& extensions);
        static void CreateDevice(const std::vector<const char*>& extensions);
        static QueueFamilies QueryDeviceQueueFamilies(VkPhysicalDevice);        
        static void SetSurface(VkSurfaceKHR surface);


        static auto Instance()               { return _instance; }
        static auto DebugMessenger()         { return _debugMessenger; }
        static auto Surface()                { return _surface; }
        static auto PhysicalDevice()         { return _physicalDevice; }  
        static auto Device()                 { return _device; }
        static auto GraphicsQueue()          { return _graphicsQueue; }
        static auto PresentationQueue()      { return _presentationQueue; }
        static auto TransferQueue()          { return _transferQueue; }
        static auto GraphicsQueueIndex()     { return _queueFamilyIndices.graphics.value(); }
        static auto PresentationQueueIndex() { return _queueFamilyIndices.presentation.value(); }
        static auto TransferQueueIndex()     { return _queueFamilyIndices.transfer.value(); }

        static const auto& PhysicalMemProperties() { return _memoryProperties; }

        static void Uninit();

        private:
            static VkInstance _instance;
            static VkDebugUtilsMessengerEXT _debugMessenger;
            static VkSurfaceKHR _surface;
            static VkPhysicalDevice _physicalDevice;
            static VkDevice _device;
            static VkQueue _graphicsQueue;
            static VkQueue _presentationQueue;
            static VkQueue _transferQueue;
            static QueueFamilies _queueFamilyIndices;
            static VkPhysicalDeviceMemoryProperties _memoryProperties;
    };    
}


#endif