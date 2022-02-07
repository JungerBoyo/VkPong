#include "VkDebug.h"


VkDebugUtilsMessengerCreateInfoEXT Pong::VkApp::InitDebugging() 
{
    VkDebugUtilsMessengerCreateInfoEXT messengerInfo{};

    messengerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    
    messengerInfo.messageSeverity = 0;
    if constexpr(VERBOSE_BIT_STATE)
        messengerInfo.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
    if constexpr(WARNING_BIT_STATE)
        messengerInfo.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
    if constexpr(ERROR_BIT_STATE)
        messengerInfo.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    messengerInfo.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT     |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT  |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT ;

    messengerInfo.pfnUserCallback = &VulkanDebugCallback;
    messengerInfo.pNext = nullptr;
    messengerInfo.pUserData = nullptr;

    return messengerInfo;
}
