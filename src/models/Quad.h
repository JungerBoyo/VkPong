#ifndef PONG_QUAD_H
#define PONG_QUAD_H

#include <array>
#include <glm/vec2.hpp>
#include <glm/mat2x2.hpp>

namespace Pong
{
    struct Vertex
    {
        enum : uint32_t 
        { 
            stride = 16,
            position_offset = 0,
            texCoord_offset = 8
        };

        glm::vec2 position;
        glm::vec2 texCoord;
    };

    struct Quad
    {
        static constexpr std::array<Vertex, 6> vertices
        {{
            {{-1.f , 1.f}, {0.f, 1.f}},// + ----- +
            {{-1.f ,-1.f}, {.0f, 0.f}},// |
            {{ 1.f ,-1.f}, {1.f, 0.f}},// +

            {{ 1.f ,-1.f}, {1.f, 0.f}},//         +
            {{ 1.f , 1.f}, {1.f, 1.f}},//         |
            {{-1.f , 1.f}, {0.f, 1.f}} // + ----- +  
        }};

        enum : uint32_t
        {
            vertex_data_size = sizeof(vertices),
            transform_data_size = 32
        };

        glm::vec2 translation{0.f, 0.f};
        glm::vec2 scaling{1.f, 1.f};
        glm::mat2 rotation{1.f};
    };
}

#endif