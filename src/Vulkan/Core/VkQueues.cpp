#include "VkMain.h"

#include <vector>


Pong::VkApp::QueueFamilies Pong::VkApp::VkCore::QueryDeviceQueueFamilies(VkPhysicalDevice device) 
{
    QueueFamilies families;

    uint32_t propsCount{0};
    vkGetPhysicalDeviceQueueFamilyProperties(device, &propsCount, nullptr);

    std::vector<VkQueueFamilyProperties> properties(propsCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &propsCount, properties.data());

    int32_t i{0};
    for(const auto& property : properties)
    {
        VkBool32 presentationSupport{false};
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, VkCore::Surface(), &presentationSupport);
        if(presentationSupport)
        {
            families.presentation = i;
            if(property.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                families.graphics = i;
            }
        }
        else if(property.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            if(!families.graphics.has_value())
                families.graphics = i;
        }
        else if(property.queueFlags & VK_QUEUE_TRANSFER_BIT)
        {
            families.transfer = i;
        }

        ++i;
    }

    if(!families.transfer.has_value() && families.graphics.has_value())
        families.transfer = families.graphics.value();

    return families;
}
