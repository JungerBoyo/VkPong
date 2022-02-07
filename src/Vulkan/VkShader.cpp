#include "VkShader.h"
#include "VkMain.h"
#include "ErrorLog.h"

#include <fstream>
#include <vector>

Pong::VkApp::ShaderModule::ShaderModule(std::string_view path)
{
    std::ifstream binStream(path.data(), std::ios::binary|std::ios::ate);   

    auto size = static_cast<size_t>(binStream.tellg());
    std::vector<char> code(size);

    binStream.seekg(0);
    binStream.read(code.data(), size);

    binStream.close();

    VkShaderModuleCreateInfo moduleInfo{};
    moduleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleInfo.codeSize = size;
    moduleInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    if(vkCreateShaderModule(VkCore::Device(), &moduleInfo, nullptr, &_shModule) != VK_SUCCESS)
    {
        LOG_ARGS("failed creating shader module from {}", path);
    }
}


Pong::VkApp::ShaderModule::~ShaderModule() 
{
    vkDestroyShaderModule(VkCore::Device(), _shModule, nullptr);
}

