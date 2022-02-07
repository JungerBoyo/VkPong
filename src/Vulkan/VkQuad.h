#ifndef PONG_VKAPP_QUAD_H
#define PONG_VKAPP_QUAD_H

#include <vulkan/vulkan.h>
 
#include "Quad.h"
#include "VkMemoryAllocator.h"

namespace Pong::VkApp
{
    struct Quad
    {   
        Quad();

        void Delete();

        auto vbo() const { return _vbo; }

        static constexpr VkVertexInputBindingDescription BindingDesc
        {
            0,                          // uint32_t
            Vertex::stride,             // stride
            VK_VERTEX_INPUT_RATE_VERTEX // inputRate
        };

        static constexpr std::array<VkVertexInputAttributeDescription, 2> AttribDesc
        {{
            ///position
            {
                0,                          // location
                0,                          // binding
                VK_FORMAT_R32G32_SFLOAT,    // format
                Vertex::position_offset     // offset
            },
            ///texCoord
            {
                1,                          // location
                0,                          // binding
                VK_FORMAT_R32G32_SFLOAT,    // format
                Vertex::texCoord_offset     // offset
            }
        }};

        private:   
            VkBuffer _vbo;  
            StagingMemoryAllocator _allocator{};
    };
}

#endif