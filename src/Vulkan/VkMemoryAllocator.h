#ifndef PONG_VKAPP_MEMORYALLOCATOR_H
#define PONG_VKAPP_MEMORYALLOCATOR_H

#include <vulkan/vulkan.h>

#include <vector>
#include <optional>

#include "VkCommands.h"

namespace Pong::VkApp
{
    /*
        Allocators dont bind buffers/images. Binding = AcquireMemory => bind yourself
    */

    struct MemChunk
    { 
        const VkDeviceSize size;
        const VkDeviceSize offset;
    };  

    struct VisibleMemoryAllocator
    {
        VisibleMemoryAllocator() = default;
        virtual void Allocate(VkMemoryPropertyFlags memProps, VkMemoryRequirements memDetails);

        /// memory + offset
        std::tuple<VkDeviceMemory, VkDeviceSize> AcquireMemory(VkDeviceSize size);

        /// offset should be aquired from AcquireMemory(...)
        void UploadData(const void* data, VkDeviceSize offset, VkDeviceSize size, bool flush = true);
       
        virtual void Delete() const;

        protected:
            VkMemoryRequirements _memDetails;
            VkMemoryPropertyFlags _memProps;
            std::vector<MemChunk> _memChunks; //  storing all mem chunks to allow for deletion and defragmetation in the future
            VkDeviceMemory _mem; 
    };

    struct StagingMemoryAllocator : public VisibleMemoryAllocator
    {   
        StagingMemoryAllocator();
        void Allocate(VkMemoryPropertyFlags memProps, VkMemoryRequirements memDetails) override;

        void UploadData(const void* data, VkDeviceSize offset, VkDeviceSize size, VkBuffer dstBuffer, VkAccessFlags dstAccess);
        void UploadData(const void* pixelData, VkDeviceSize offset, VkExtent2D dstImgSize, uint32_t dstImgChannelNum, VkImageAspectFlags dstImgAspect, VkImage dstImg, VkPipelineStageFlags dstImgStage, VkImageLayout dstImgLayout);

        void Delete() const override;

        private:
            SimpleCmdBufferAllocator _copyCmdBuffAlloc;
            VkFence _copyCmdFence;
            VkMemoryPropertyFlags _stagingMemProps{0};
            VkDeviceMemory _stagingMem{VK_NULL_HANDLE};
            VkBuffer _stagingBuff{VK_NULL_HANDLE};
    };
}

#endif