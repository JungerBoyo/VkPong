#include "VkMemoryAllocator.h"
#include "VkMain.h"

#include "ErrorLog.h"

int32_t FindProperties(const VkPhysicalDeviceMemoryProperties& pMemProperties, uint32_t memTypeBits, VkMemoryPropertyFlags requiredProperties);

Pong::VkApp::StagingMemoryAllocator::StagingMemoryAllocator()
    : _copyCmdBuffAlloc(VkCore::TransferQueueIndex(), 1, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT|VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT)
{
    VkFenceCreateInfo fenceInfo{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    if(vkCreateFence(VkCore::Device(), &fenceInfo, nullptr, &_copyCmdFence) != VK_SUCCESS)
    {
        LOG("failed to create fence");
    }
}

void Pong::VkApp::VisibleMemoryAllocator::Allocate(VkMemoryPropertyFlags memProps, VkMemoryRequirements memDetails)
{   
    _memDetails = memDetails;

    const auto& pMemProps = VkCore::PhysicalMemProperties();
    auto memoryIndex = FindProperties(pMemProps, memDetails.memoryTypeBits, memProps);
   
    if(memoryIndex == -1)
    {
        LOG("couldn't create allocator, memory requirements not fulfilled");
        return;
    }
    _memProps = pMemProps.memoryTypes[memoryIndex].propertyFlags;
 
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.pNext = nullptr;
    allocInfo.allocationSize = memDetails.size;
    allocInfo.memoryTypeIndex = memoryIndex;

    if(vkAllocateMemory(VkCore::Device(), &allocInfo, nullptr, &_mem) != VK_SUCCESS)
    {
        LOG("failed to allocate memory");
        return;
    }
}

void Pong::VkApp::StagingMemoryAllocator::Allocate(VkMemoryPropertyFlags memProps, VkMemoryRequirements memDetails)
{
    VisibleMemoryAllocator::Allocate(memProps, memDetails);

    if(!(_memProps & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))
    {
        LOG("staging allocator not device local");
    }

    VkBufferCreateInfo buffInfo{};
    buffInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffInfo.queueFamilyIndexCount = 0;
    buffInfo.pQueueFamilyIndices = nullptr;
    buffInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    buffInfo.size = memDetails.size;
    buffInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

    if(vkCreateBuffer(VkCore::Device(), &buffInfo, nullptr, &_stagingBuff) != VK_SUCCESS)
    {
        LOG("failed to create staging buffer for memory allocator, allocator won't be created");
        return;
    }

    VkMemoryRequirements buffMemRequirements;
    vkGetBufferMemoryRequirements(VkCore::Device(), _stagingBuff, &buffMemRequirements);
    auto stagingMemoryIndex = FindProperties(VkCore::PhysicalMemProperties(), buffMemRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    if(stagingMemoryIndex == -1)
    {
        LOG("couldn't create staging memory for allocator, memory requirements not fulfilled");
        return;
    }
        
    _stagingMemProps = VkCore::PhysicalMemProperties().memoryTypes[stagingMemoryIndex].propertyFlags;

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.pNext = nullptr;
    allocInfo.allocationSize = memDetails.size;
    allocInfo.memoryTypeIndex = stagingMemoryIndex;
    
    if(vkAllocateMemory(VkCore::Device(), &allocInfo, nullptr, &_stagingMem) != VK_SUCCESS)
    {
        LOG("failed to allocate memory for staging buffer");
        return;
    }

    vkBindBufferMemory(VkCore::Device(), _stagingBuff, _stagingMem, 0);
}


std::tuple<VkDeviceMemory, VkDeviceSize> Pong::VkApp::VisibleMemoryAllocator::AcquireMemory(VkDeviceSize size)
{
    const auto alignedSize = static_cast<VkDeviceSize>(_memDetails.alignment*std::ceil(static_cast<float>(size)/static_cast<float>(_memDetails.alignment))); 
    const auto offset = _memChunks.empty() ? 0 : _memChunks.back().offset + _memChunks.back().size;
    if(offset + alignedSize > _memDetails.size)
    {
        LOG_ARGS("memory size exceeded, size {}, wanted to upload data from {} to {}", _memDetails.size, offset, offset + alignedSize);
        return {VK_NULL_HANDLE, UINT64_MAX};
    }

    _memChunks.emplace_back(alignedSize, offset);

    return {_mem, offset};
}

void Pong::VkApp::VisibleMemoryAllocator::UploadData(const void* data, VkDeviceSize offset, VkDeviceSize size, bool flush)
{
    if(offset + size > _memDetails.size)
    {
        LOG_ARGS("memory size exceeded, size {}, wanted to upload data from {} to {}", _memDetails.size, size, size + offset);
        return;
    }

    if(_memProps & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
    {
        void *mappedMemory;
        if(vkMapMemory(VkCore::Device(), _mem, offset, size, 0, &mappedMemory) != VK_SUCCESS)
        {
            LOG("failed to map memory");
            return;
        }

        memcpy(mappedMemory, data, size);

        if(flush && !(_memProps & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
        {
            VkMappedMemoryRange flushRange{};
            flushRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
            flushRange.memory = _mem;
            flushRange.offset = offset;
            flushRange.size = size;
        
            vkFlushMappedMemoryRanges(VkCore::Device(), 1, &flushRange);
        }

        vkUnmapMemory(VkCore::Device(), _mem);
    }
    else
    {
        LOG("memory is not visible");
    }
}

void Pong::VkApp::StagingMemoryAllocator::UploadData(const void* pixelData, VkDeviceSize offset, VkExtent2D dstImgSize, uint32_t dstImgChannelNum, VkImageAspectFlags dstImgAspect, VkImage dstImg, VkPipelineStageFlags dstImgStage, VkImageLayout dstImgLayout)
{
    auto size = dstImgSize.width * dstImgSize.height * dstImgChannelNum;
    if(offset + size > _memDetails.size) 
    {
        LOG_ARGS("memory size exceeded, size {}, wanted to upload data from {} to {}", _memDetails.size, size, size + offset);
        return;
    }

    if(!(_memProps & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))
    {
        LOG("warning, memory is not device local. Redundant staging");
    }
    
    void *mappedMemory;
    if(vkMapMemory(VkCore::Device(), _stagingMem, offset, size, 0, &mappedMemory) != VK_SUCCESS)
    {
        LOG("failed to map memory");
        return;
    }

    memcpy(mappedMemory, pixelData, size);

    if(!(_stagingMemProps & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
    {
        VkMappedMemoryRange flushRange{};
        flushRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        flushRange.memory = _stagingMem;
        flushRange.offset = offset;
        flushRange.size = size;
    
        vkFlushMappedMemoryRanges(VkCore::Device(), 1, &flushRange);
    }

    vkUnmapMemory(VkCore::Device(), _stagingMem);

    VkCommandBuffer cmdBuffer{VK_NULL_HANDLE};
    {
        auto scope = _copyCmdBuffAlloc.RecordBuffer(0, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
        cmdBuffer = scope.getRecordedCmdBuffer();

        VkImageMemoryBarrier barrierToTransfer{};
        barrierToTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrierToTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barrierToTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrierToTransfer.srcAccessMask = 0;
        barrierToTransfer.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrierToTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrierToTransfer.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrierToTransfer.subresourceRange.aspectMask = dstImgAspect;
        barrierToTransfer.subresourceRange.baseArrayLayer = 0;
        barrierToTransfer.subresourceRange.layerCount = 1;
        barrierToTransfer.subresourceRange.baseMipLevel = 0;
        barrierToTransfer.subresourceRange.levelCount = 1;
        barrierToTransfer.image = dstImg;

        vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrierToTransfer);
    

        VkBufferImageCopy buffImgCopy{};
        buffImgCopy.bufferOffset = offset;
        buffImgCopy.bufferImageHeight = 0;
        buffImgCopy.bufferRowLength = 0;
        buffImgCopy.imageExtent.width = dstImgSize.width;
        buffImgCopy.imageExtent.height = dstImgSize.height;
        buffImgCopy.imageExtent.depth = 1;
        buffImgCopy.imageSubresource.aspectMask = dstImgAspect;
        buffImgCopy.imageSubresource.baseArrayLayer = 0;
        buffImgCopy.imageSubresource.layerCount = 1;
        buffImgCopy.imageSubresource.mipLevel = 0;

        vkCmdCopyBufferToImage(cmdBuffer, _stagingBuff, dstImg, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &buffImgCopy);

        
        VkImageMemoryBarrier barrierFromTransfer{};
        barrierFromTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrierFromTransfer.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrierFromTransfer.newLayout = dstImgLayout;
        barrierFromTransfer.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrierFromTransfer.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrierFromTransfer.dstAccessMask = 0;
        barrierFromTransfer.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrierFromTransfer.subresourceRange.aspectMask = dstImgAspect;
        barrierFromTransfer.subresourceRange.baseArrayLayer = 0;
        barrierFromTransfer.subresourceRange.layerCount = 1;
        barrierFromTransfer.subresourceRange.baseMipLevel = 0;
        barrierFromTransfer.subresourceRange.levelCount = 1;
        barrierFromTransfer.image = dstImg;

        vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, dstImgStage, 0, 0, nullptr, 0, nullptr, 1, &barrierFromTransfer);
    }

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuffer;
    submitInfo.pWaitDstStageMask = nullptr;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = nullptr;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitDstStageMask = nullptr;
    submitInfo.pWaitSemaphores = nullptr;

    if(vkQueueSubmit(VkCore::TransferQueue(), 1, &submitInfo, _copyCmdFence) != VK_SUCCESS)
    {
        LOG("failed to submit copy cmd to transfer queue");
    }

    vkWaitForFences(VkCore::Device(), 1, &_copyCmdFence, VK_TRUE, UINT64_MAX);
    
    _copyCmdBuffAlloc.ResetCommandBuffers();
    vkResetFences(VkCore::Device(), 1, &_copyCmdFence);
}

void Pong::VkApp::StagingMemoryAllocator::UploadData(const void* data, VkDeviceSize offset, VkDeviceSize size, VkBuffer dstBuffer, VkAccessFlags dstAccess)
{
    if(offset + size > _memDetails.size)
    {
        LOG_ARGS("memory size exceeded, size {}, wanted to upload data from {} to {}", _memDetails.size, size, size + offset);
        return;
    }

    if(!(_memProps & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))
    {
        LOG("warning, memory is not device local. Redundant staging");
    }
    
    void *mappedMemory;
    if(vkMapMemory(VkCore::Device(), _stagingMem, offset, size, 0, &mappedMemory) != VK_SUCCESS)
    {
        LOG("failed to map memory");
        return;
    }

    memcpy(mappedMemory, data, size);

    if(!(_stagingMemProps & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
    {
        VkMappedMemoryRange flushRange{};
        flushRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        flushRange.memory = _stagingMem;
        flushRange.offset = offset;
        flushRange.size = size;
    
        vkFlushMappedMemoryRanges(VkCore::Device(), 1, &flushRange);
    }

    vkUnmapMemory(VkCore::Device(), _stagingMem);

    VkCommandBuffer cmdBuffer{VK_NULL_HANDLE};
    {
        auto scope = _copyCmdBuffAlloc.RecordBuffer(0, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
        cmdBuffer = scope.getRecordedCmdBuffer();

        VkBufferCopy buffCopy{};
        buffCopy.dstOffset = offset;
        buffCopy.srcOffset = offset;
        buffCopy.size = size;

        vkCmdCopyBuffer(cmdBuffer, _stagingBuff, dstBuffer, 1, &buffCopy);
    }

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuffer;
    submitInfo.pWaitDstStageMask = nullptr;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = nullptr;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitDstStageMask = nullptr;
    submitInfo.pWaitSemaphores = nullptr;

    if(vkQueueSubmit(VkCore::TransferQueue(), 1, &submitInfo, _copyCmdFence) != VK_SUCCESS)
    {
        LOG("failed to submit copy cmd to transfer queue");
    }

    vkWaitForFences(VkCore::Device(), 1, &_copyCmdFence, VK_TRUE, UINT64_MAX);
    
    _copyCmdBuffAlloc.ResetCommandBuffers();
    vkResetFences(VkCore::Device(), 1, &_copyCmdFence);
}

void Pong::VkApp::VisibleMemoryAllocator::Delete() const
{
    vkFreeMemory(VkCore::Device(), _mem, nullptr);
}

void Pong::VkApp::StagingMemoryAllocator::Delete() const 
{   
    VisibleMemoryAllocator::Delete();
    vkDestroyFence(VkCore::Device(), _copyCmdFence, nullptr);
    _copyCmdBuffAlloc.Delete();
    vkDestroyBuffer(VkCore::Device(), _stagingBuff, nullptr);
    vkFreeMemory(VkCore::Device(), _stagingMem, nullptr);
}

int32_t FindProperties(const VkPhysicalDeviceMemoryProperties& pMemProperties, uint32_t memTypeBits, VkMemoryPropertyFlags requiredProperties)
{
    for (uint32_t memIndex = 0; memIndex < pMemProperties.memoryTypeCount; ++memIndex) 
    {
        const bool isRequiredMemoryType = memTypeBits & (1 << memIndex);
        const auto properties = pMemProperties.memoryTypes[memIndex].propertyFlags;
        const bool hasRequiredProperties = (properties & requiredProperties) == requiredProperties;

        if (isRequiredMemoryType && hasRequiredProperties)
            return static_cast<int32_t>(memIndex);
    }

    return -1;
}






