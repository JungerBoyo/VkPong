#ifndef PONG_VKAPP_PIPELINE_H
#define PONG_VKAPP_PIPELINE_H

#include <string_view>
#include <vector>
#include <vulkan/vulkan.h>

namespace Pong::VkApp
{
    struct Pipeline
    {
        Pipeline() = default;
        void Set(VkRenderPass renderPass, std::string_view vShPath, std::string_view fShPath, const std::vector<VkDescriptorSetLayout>& descSetLayouts, 
                 const std::vector<VkPushConstantRange>& pushConstantRanges);
        void Delete() const;

        auto getLayout() const { return _pipelineLayout; }

        operator VkPipeline() const
        {
            return _pipeline;
        }

        private:
            VkPipeline _pipeline{VK_NULL_HANDLE};
            VkPipelineLayout _pipelineLayout{VK_NULL_HANDLE};
    };
}

#endif