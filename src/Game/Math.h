#ifndef PONG_MATH_H
#define PONG_MATH_H

#include <glm/vec2.hpp>

namespace Pong
{
    void rotate2D(glm::vec2& vec, float angleInRadians);
    void collide(glm::vec2 collider00, glm::vec2 collider11, glm::vec2 collidingSrc, glm::vec2 collidingDir, glm::vec2& reflectMe);
    bool intersect(glm::vec2 c1, glm::vec2 c2, glm::vec2 C1, glm::vec2 C2);
}

#endif