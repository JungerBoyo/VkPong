#include "VkDescriptorSetsAllocator.h"
#include "VkMain.h"
#include "ErrorLog.h"

Pong::VkApp::DescriptorSetsAllocator::DescriptorSetsAllocator(const std::vector<VkDescriptorPoolSize>& sizes, uint32_t maxSets) 
{
    VkDescriptorPoolCreateInfo descSetPoolInfo{};
    descSetPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descSetPoolInfo.maxSets = maxSets;
    descSetPoolInfo.poolSizeCount = sizes.size();
    descSetPoolInfo.pPoolSizes = sizes.data();
    
    if(vkCreateDescriptorPool(VkCore::Device(), &descSetPoolInfo, nullptr, &_descPool) != VK_SUCCESS)
    {
        LOG("failed to create descriptor pool");
    }
}

void Pong::VkApp::DescriptorSetsAllocator::Allocate(const std::vector<VkDescriptorSetLayoutBinding>& layoutBindings) 
{
    VkDescriptorSetLayoutCreateInfo descSetLayoutInfo{};
    descSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descSetLayoutInfo.bindingCount = layoutBindings.size();
    descSetLayoutInfo.pBindings = layoutBindings.data();    

    VkDescriptorSetLayout setLayout;
    if(vkCreateDescriptorSetLayout(VkCore::Device(), &descSetLayoutInfo, nullptr, &setLayout) != VK_SUCCESS)
    {
        LOG("failed to create descriptor set layout");
    }

    VkDescriptorSetAllocateInfo descSetAllocInfo{};
    descSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descSetAllocInfo.descriptorPool = _descPool;
    descSetAllocInfo.descriptorSetCount = 1;
    descSetAllocInfo.pSetLayouts = &setLayout;
    
    VkDescriptorSet descSet;
    if(vkAllocateDescriptorSets(VkCore::Device(), &descSetAllocInfo, &descSet) != VK_SUCCESS)
    {
        LOG("failed to create descriptor set");
    }

    _descriptorSets.push_back(descSet);
    _descriptorLayouts.push_back(setLayout);
}

void Pong::VkApp::DescriptorSetsAllocator::Delete()
{
    for(const auto layout : _descriptorLayouts) 
        vkDestroyDescriptorSetLayout(VkCore::Device(), layout, nullptr);  
    
    vkDestroyDescriptorPool(VkCore::Device(), _descPool, nullptr);
}

void Pong::VkApp::DescriptorSetsAllocator::Update(std::vector<uint32_t> setIndicies, std::vector<uint32_t> bindings, const std::vector<VkDescriptorType>& descTypes,  std::vector<const std::vector<VkDescriptorImageInfo>*> imgInfos) 
{
    std::vector<VkWriteDescriptorSet> writesDescSets(descTypes.size());
    
    size_t i{0};
    for(auto& writeDescSet : writesDescSets)
    {
        writeDescSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescSet.descriptorCount = imgInfos[i]->size();
        writeDescSet.descriptorType = descTypes[i];
        writeDescSet.dstArrayElement = 0;
        writeDescSet.dstBinding = bindings[i];
        writeDescSet.dstSet = _descriptorSets[setIndicies[i]];
        writeDescSet.pBufferInfo = nullptr;
        writeDescSet.pTexelBufferView = nullptr;
        writeDescSet.pImageInfo = imgInfos[i]->data();

        ++i;
    }
    
    vkUpdateDescriptorSets(VkCore::Device(), writesDescSets.size(), writesDescSets.data(), 0, nullptr);
}
