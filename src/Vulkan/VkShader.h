#ifndef PONG_VKAPP_SHADER_H
#define PONG_VKAPP_SHADER_H

#include <string_view>
#include <vulkan/vulkan.h>

namespace Pong::VkApp
{
    struct ShaderModule
    {
        ShaderModule(std::string_view path);
        ~ShaderModule();

        
        operator VkShaderModule() const
        {
            return _shModule;
        }

        private:
            VkShaderModule _shModule;
    };
}

#endif