#ifndef PONG_PONG_H
#define PONG_PONG_H

#include <array>

#include "Quad.h"

namespace Pong
{
    struct Game
    {
        Game(float p1XPos, float p1Speed, glm::vec2 p1Scaling,
             float p2XPos, float p2Speed, glm::vec2 p2Scaling, 
             float ballSpeed, glm::vec2 ballScaling);

        void ControlCallback(int32_t key);
        void Update(float updateRate);

        const auto& getObjectsTransforms() const { return _objectTransforms; }

        private:
            struct MovingObject
            {
                auto move() const 
                {
                    return speed * mvVector;
                }

                glm::vec2 mvVector{0.f, 0.f};
                float speed;
            };

            MovingObject _mvP1;
            MovingObject _mvP2;
            MovingObject _mvBall;

            enum : uint32_t { background_Ob, p1_Ob, p2_Ob, ball_Ob };
            std::array<Quad, 4> _objectTransforms;
    };
}

#endif