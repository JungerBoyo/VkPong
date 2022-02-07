#include "VkCommands.h"
#include "VkMain.h"
#include "ErrorLog.h"

Pong::VkApp::SimpleCmdBufferAllocator::RecordScope::RecordScope(VkCommandBufferBeginInfo beginInfo, VkCommandBuffer cmdBuffer)
    : _cmdBuffer(cmdBuffer)
{
    vkBeginCommandBuffer(_cmdBuffer, &beginInfo);
}

Pong::VkApp::SimpleCmdBufferAllocator::RecordScope::~RecordScope() 
{
    if(vkEndCommandBuffer(_cmdBuffer) != VK_SUCCESS)
    {
        LOG("Failed to record command buffer");
    }   
}

Pong::VkApp::SimpleCmdBufferAllocator::RecordScope Pong::VkApp::SimpleCmdBufferAllocator::RecordBuffer(size_t i, VkCommandBufferUsageFlags usage)
{
    VkCommandBufferBeginInfo cmdBuffBeginInfo{};
    cmdBuffBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBuffBeginInfo.pNext = nullptr;
    cmdBuffBeginInfo.flags = usage;
    cmdBuffBeginInfo.pInheritanceInfo = nullptr;

    return {cmdBuffBeginInfo, _cmdBuffers[i]};
}


Pong::VkApp::SimpleCmdBufferAllocator::SimpleCmdBufferAllocator(uint32_t queueFamilyIndex, uint32_t cmdBuffersCount, VkCommandPoolCreateFlags flags, VkCommandBufferLevel level)
{
    VkCommandPoolCreateInfo cmdPoolInfo{};
    cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolInfo.pNext = nullptr;
    cmdPoolInfo.flags = flags;
    cmdPoolInfo.queueFamilyIndex = queueFamilyIndex;

    if(vkCreateCommandPool(VkCore::Device(), &cmdPoolInfo, nullptr, &_cmdPool) != VK_SUCCESS)
    {
        LOG_ARGS("failed to create command pool for family {}", queueFamilyIndex);
        return;
    }

    VkCommandBufferAllocateInfo cmdBuffAllocInfo{};
    cmdBuffAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdBuffAllocInfo.level = level;
    cmdBuffAllocInfo.commandBufferCount = cmdBuffersCount;
    cmdBuffAllocInfo.commandPool = _cmdPool;
    cmdBuffAllocInfo.pNext = nullptr;

    _cmdBuffers.resize(cmdBuffersCount);
    if(vkAllocateCommandBuffers(VkCore::Device(), &cmdBuffAllocInfo, _cmdBuffers.data()) != VK_SUCCESS)
    {
        LOG("failed to allocate command buffers");
    }
}

void Pong::VkApp::SimpleCmdBufferAllocator::ResetCommandBuffers() const
{
    vkResetCommandPool(VkCore::Device(), _cmdPool, 0);
}

void Pong::VkApp::SimpleCmdBufferAllocator::Delete() const 
{
    vkDestroyCommandPool(VkCore::Device(), _cmdPool, nullptr);
}

