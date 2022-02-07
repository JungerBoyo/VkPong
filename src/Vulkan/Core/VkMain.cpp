#include "VkMain.h"
#include "VkDebug.h"
#include "ExtFunctions.h"

#include <fmt/format.h>
#include <queue>

VkInstance Pong::VkApp::VkCore::_instance{VK_NULL_HANDLE};
VkDebugUtilsMessengerEXT Pong::VkApp::VkCore::_debugMessenger{VK_NULL_HANDLE};
VkSurfaceKHR Pong::VkApp::VkCore::_surface{VK_NULL_HANDLE};
VkPhysicalDevice Pong::VkApp::VkCore::_physicalDevice{VK_NULL_HANDLE};
VkPhysicalDeviceMemoryProperties Pong::VkApp::VkCore::_memoryProperties{0};
VkDevice Pong::VkApp::VkCore::_device{VK_NULL_HANDLE};
VkQueue Pong::VkApp::VkCore::_graphicsQueue{VK_NULL_HANDLE};
VkQueue Pong::VkApp::VkCore::_presentationQueue{VK_NULL_HANDLE};
VkQueue Pong::VkApp::VkCore::_transferQueue{VK_NULL_HANDLE};
Pong::VkApp::QueueFamilies Pong::VkApp::VkCore::_queueFamilyIndices{};


static bool CheckInstanceExtSupport(const std::vector<const char*>& extensions)
{
    uint32_t extCount{0};
    vkEnumerateInstanceExtensionProperties(nullptr, &extCount, nullptr);

    std::vector<VkExtensionProperties> avExtProperties(extCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extCount, avExtProperties.data());

    for(const auto* ext : extensions)
    {
        bool extFound{false};

        for(const auto& avExt : avExtProperties)
            if(!strcmp(ext, avExt.extensionName))
            {
                extFound = true;
                break;
            }

        if(!extFound)
        {
            LOG_ARGS("extension {} not found", ext);
            return false;
        }
    }

    return true;
}
static bool CheckValidationLayers()
{
    uint32_t layerCount{0};
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> avLayerProperties(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, avLayerProperties.data());

    for(const auto* layer : Pong::VkApp::VALIDATION_LAYERS)
    {
        bool layerFound{false};

        for(const auto& avLayer : avLayerProperties)
            if(!strcmp(layer, avLayer.layerName))
            {
                layerFound = true;
                break;
            }

        if(!layerFound)
        {
            LOG_ARGS("layer {} not found", layer);
            return false;
        }
    }

    return true;
}
static bool CheckDeviceExtSupport(VkPhysicalDevice device, const std::vector<const char*>& extensions)
{
    uint32_t propsCount{0};
    vkEnumerateDeviceExtensionProperties(device, nullptr, &propsCount, nullptr);

    std::vector<VkExtensionProperties> properties(propsCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &propsCount, properties.data());

    uint32_t counter{0};
    for(const auto& property : properties)
        for(const auto& ext : extensions)
            if(!strcmp(ext, property.extensionName))
                counter++;
    
    return counter == extensions.size();
}
static auto IsDeviceSuitable(VkPhysicalDevice device, const std::vector<const char*>& extensions)
{
    auto families = Pong::VkApp::VkCore::QueryDeviceQueueFamilies(device);
    auto doesSupportExtensions = CheckDeviceExtSupport(device, extensions);

    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(device, &properties);

    bool versionOk = (VK_API_VERSION_MAJOR(properties.apiVersion) == 1) && 
                     (VK_API_VERSION_MINOR(properties.apiVersion) == 2);

    return std::make_tuple(doesSupportExtensions && families && versionOk, families);
}

void Pong::VkApp::VkCore::CreateInstance(const std::vector<const char*>& extensions) 
{
    if constexpr(VALIDATION_LAYERS_STATE)
        if(!CheckValidationLayers())
        {
            LOG("validation layers not available");
        }

    if(!CheckInstanceExtSupport(extensions))
    {
        LOG("requested instance extensions not available");
        return;
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Pong";
    appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 2, 0);
    appInfo.pEngineName = "noegine";
    appInfo.apiVersion = VK_API_VERSION_1_2;

    VkInstanceCreateInfo instanceInfo{};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.enabledExtensionCount = extensions.size();
    instanceInfo.ppEnabledExtensionNames = extensions.data();

    VkResult instanceCreationResult;
    if constexpr(VALIDATION_LAYERS_STATE)
    {
        auto debugInfo{InitDebugging()};
        instanceInfo.enabledLayerCount = VALIDATION_LAYERS.size();
        instanceInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
        instanceInfo.pNext = &debugInfo;

        instanceCreationResult = vkCreateInstance(&instanceInfo, nullptr, &_instance);

        if(CreateDebugUtilsMessengerEXT(_instance, &debugInfo, nullptr, &_debugMessenger) != VK_SUCCESS)    
        {
           LOG("failed to create debug messenger");
        }
    }
    else
    {
        instanceCreationResult = vkCreateInstance(&instanceInfo, nullptr, &_instance);
    }

    if(instanceCreationResult != VK_SUCCESS)
    {
        LOG("failed to create vulkan instance");
    }
}


void Pong::VkApp::VkCore::PickPhysicalDevice(const std::vector<const char*>& extensions)
{
    uint32_t deviceCount{0};
    vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);

    if(!deviceCount)
    {
        LOG("no gpu was found");
        return;
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());

    for(const auto& device : devices)
    {
        auto[isSuitable, queIndices] = IsDeviceSuitable(device, extensions);
        if(isSuitable)
        {
            _queueFamilyIndices = queIndices;
            _physicalDevice = device;
            vkGetPhysicalDeviceMemoryProperties(device, &_memoryProperties);
            break;
        }
    }

    if(_physicalDevice == VK_NULL_HANDLE)
    {
        LOG("no suitable gpu was found");
    }
}

void Pong::VkApp::VkCore::CreateDevice(const std::vector<const char*>& extensions) 
{
    VkDeviceCreateInfo deviceInfo{};
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.enabledExtensionCount = extensions.size();
    deviceInfo.ppEnabledExtensionNames = extensions.data();

    if constexpr(VALIDATION_LAYERS_STATE)
    {
        deviceInfo.enabledLayerCount = VALIDATION_LAYERS.size();
        deviceInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
    }

    deviceInfo.pEnabledFeatures = nullptr;
    

    std::vector<uint32_t> families{*_queueFamilyIndices.graphics};
  
    if(_queueFamilyIndices.presentation != _queueFamilyIndices.graphics)
        families.push_back(*_queueFamilyIndices.presentation);

    if(*_queueFamilyIndices.transfer != *_queueFamilyIndices.presentation &&
       *_queueFamilyIndices.transfer != *_queueFamilyIndices.graphics)
        families.push_back(*_queueFamilyIndices.transfer);

    std::vector<VkDeviceQueueCreateInfo> deviceQueueInfos(families.size());
    auto priority{1.f};
    for(uint32_t i{0}; i<families.size(); ++i)
    {
        deviceQueueInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        deviceQueueInfos[i].queueFamilyIndex = families[i];
        deviceQueueInfos[i].queueCount = 1;
        deviceQueueInfos[i].pQueuePriorities = &priority;
    }

    deviceInfo.pQueueCreateInfos = deviceQueueInfos.data();
    deviceInfo.queueCreateInfoCount = deviceQueueInfos.size();

    if(vkCreateDevice(_physicalDevice, &deviceInfo, nullptr, &_device) != VK_SUCCESS)
    {
        LOG("failed to create device");
        return;
    }

    vkGetDeviceQueue(_device, *_queueFamilyIndices.graphics, 0, &_graphicsQueue);
    vkGetDeviceQueue(_device, *_queueFamilyIndices.presentation, 0, &_presentationQueue);
    vkGetDeviceQueue(_device, *_queueFamilyIndices.transfer, 0, &_transferQueue);
}

void Pong::VkApp::VkCore::Uninit()
{
    if(_device != VK_NULL_HANDLE)
    {
        vkDeviceWaitIdle(_device);
        vkDestroyDevice(_device, nullptr);
    }
        
    if constexpr(VALIDATION_LAYERS_STATE)
        if(_debugMessenger != VK_NULL_HANDLE)
            DestroyDebugUtilsMessengerEXT(_instance, _debugMessenger, nullptr);

    if(_surface != VK_NULL_HANDLE)
        vkDestroySurfaceKHR(_instance, _surface, nullptr);

    if(_instance != VK_NULL_HANDLE)
        vkDestroyInstance(_instance, nullptr);
}

void Pong::VkApp::VkCore::SetSurface(VkSurfaceKHR surface) 
{
    _surface = surface;   
}
