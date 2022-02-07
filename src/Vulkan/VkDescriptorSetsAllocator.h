#ifndef PONG_VKAPP_VKDESCRIPTORSETSALLOCATOR_H
#define PONG_VKAPP_VKDESCRIPTORSETSALLOCATOR_H

#include <vulkan/vulkan.h>
#include <vector>

namespace Pong::VkApp
{
    struct DescriptorSetsAllocator
    {
        DescriptorSetsAllocator(const std::vector<VkDescriptorPoolSize>& sizes, uint32_t maxSets);
        void Allocate(const std::vector<VkDescriptorSetLayoutBinding>& layoutBindings);
        void Update(std::vector<uint32_t> setIndicies, std::vector<uint32_t> bindings, const std::vector<VkDescriptorType>& descType, std::vector<const std::vector<VkDescriptorImageInfo>*> imgInfos);
                    
        void Delete();

        const auto& getLayouts() const { return _descriptorLayouts; }
        const auto& getSets() const { return _descriptorSets; }

        private:
            VkDescriptorPool _descPool{VK_NULL_HANDLE};
            std::vector<VkDescriptorSetLayout> _descriptorLayouts;
            std::vector<VkDescriptorSet> _descriptorSets;
    };
}

#endif 