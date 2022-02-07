#ifndef PONG_VKAPP_EXTFUNCTIONS_H
#define PONG_VKAPP_EXTFUNCTIONS_H
 
#include <vulkan/vulkan.h>

namespace Pong::VkApp
{
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                          const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
}


#endif