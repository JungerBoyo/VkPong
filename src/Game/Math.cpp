#include "Math.h"
#include <glm/mat2x2.hpp>

void Pong::rotate2D(glm::vec2& vec, float angleInRadians) 
{
    glm::mat2 rotMat;

    rotMat[0][0] = cosf32(angleInRadians);
    rotMat[1][0] = sinf32(angleInRadians);
    rotMat[0][1] = -rotMat[1][0];
    rotMat[1][1] = rotMat[0][0];

    vec = rotMat * vec;
}

void Pong::collide(glm::vec2 collider00, glm::vec2 collider11, glm::vec2 collidingSrc, glm::vec2 collidingDir, glm::vec2& reflectMe) 
{
    glm::vec2 collidingDst = collidingSrc + collidingDir;

    if(intersect(collidingSrc, collidingDst, collider00, {collider11.x, collider00.y}))
        reflectMe = glm::reflect(reflectMe, {.0f, 1.f});   
            
    if(intersect(collidingSrc, collidingDst, {collider11.x, collider00.y}, collider11))
        reflectMe = glm::reflect(reflectMe, {1.f, 0.f});

    if(intersect(collidingSrc, collidingDst, {collider00.x, collider11.y}, collider11))
        reflectMe = glm::reflect(reflectMe, {0.f,-1.f});

    if(intersect(collidingSrc, collidingDst, collider00, {collider00.x, collider11.y}))
        reflectMe = glm::reflect(reflectMe, {-1.f, 0.f});
}

bool Pong::intersect(glm::vec2 c1, glm::vec2 c2, glm::vec2 C1, glm::vec2 C2) 
{
    ///additional check was needed (float precission too low????)
    if(glm::length(c1 - c2) + glm::length(C1 - C2) < glm::length(((c1+c2)/2.f) - ((C1+C2)/2.f)))
        return false;

    float x = c1.y - c2.y;
    float y = c2.y - C2.y;
    float z = C1.y - C2.y;

    float a = C1.x - C2.x;
    float b = C2.x - c2.x;
    float c = c1.x - c2.x;

    float s = (a*y + b*z)/(z*c - a*x);

    return s >= 0.f && s <= 1.f; 
}
