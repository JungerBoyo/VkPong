#ifndef PONG_GAMESTARTUP_H
#define PONG_GAMESTARTUP_H

#include "Img.h"

#include <glm/vec2.hpp>

#include <string_view>
#include <array>

namespace Pong
{
    struct GameStartupData
    {
        GameStartupData(std::string_view modJSONFilePath);

        uint32_t framesInFlight;

        float lhsPlayerHorizontalPos;
        glm::vec2 lhsPlayerScaling;
        float lhsPlayerSpeed;
        
        float rhsPlayerHorizontalPos;
        glm::vec2  rhsPlayerScaling;
        float rhsPlayerSpeed;
        
        glm::vec2  ballScaling;
        float ballSpeed;

        std::array<Img, 4> textures;
    };
}

#endif