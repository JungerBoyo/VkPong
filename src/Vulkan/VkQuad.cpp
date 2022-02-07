#include "VkQuad.h"
#include "VkMain.h"
#include "ErrorLog.h"

Pong::VkApp::Quad::Quad()
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.flags = 0;
    bufferInfo.pNext = nullptr;
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    bufferInfo.size = Pong::Quad::vertex_data_size;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferInfo.pQueueFamilyIndices = nullptr;
    bufferInfo.queueFamilyIndexCount = 0;

    if(vkCreateBuffer(VkCore::Device(), &bufferInfo, nullptr, &_vbo) != VK_SUCCESS)
    {
        LOG("failed to create quad vertex buffer object");
        return;
    }     

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(VkCore::Device(), _vbo, &memRequirements);
    _allocator.Allocate(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memRequirements);

    auto[mem, offset] = _allocator.AcquireMemory(Pong::Quad::vertex_data_size);

    vkBindBufferMemory(VkCore::Device(), _vbo, mem, offset);

    _allocator.UploadData(Pong::Quad::vertices.data(), offset, Pong::Quad::vertex_data_size, _vbo, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT);
    //_allocator.UploadData(Pong::Quad::vertices.data(), offset, Pong::Quad::size);
}

void Pong::VkApp::Quad::Delete() 
{
    vkDestroyBuffer(VkCore::Device(), _vbo, nullptr);
    _allocator.Delete();
}
