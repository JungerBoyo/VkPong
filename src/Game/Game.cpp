#include "Game.h"
#include "Math.h"

#include <GLFW/glfw3.h>


Pong::Game::Game(float p1XPos, float p1Speed, glm::vec2 p1Scaling,
                 float p2XPos, float p2Speed, glm::vec2 p2Scaling, 
                 float ballSpeed, glm::vec2 ballScaling)
{
    _objectTransforms[p1_Ob].translation.x = p1XPos;
    _objectTransforms[p1_Ob].scaling = p1Scaling;

    _objectTransforms[p2_Ob].translation.x = p2XPos;
    _objectTransforms[p2_Ob].scaling = p2Scaling;

    _objectTransforms[ball_Ob].scaling = ballScaling;

    _mvP1.speed = p1Speed;
    _mvP2.speed = p2Speed;
    _mvBall.speed = ballSpeed;

    _mvBall.mvVector = {1.f, 0.f};
    rotate2D(_mvBall.mvVector, -M_PI/5);
}


void Pong::Game::ControlCallback(int32_t key) 
{
    switch(key)
    {
        case GLFW_KEY_W: _mvP1.mvVector.y = -1.f; break;
        case GLFW_KEY_S: _mvP1.mvVector.y =  1.f; break;
    
        case GLFW_KEY_UP:   _mvP2.mvVector.y = -1.f; break; 
        case GLFW_KEY_DOWN: _mvP2.mvVector.y =  1.f; break; 

        default: break;
    }    
}

void Pong::Game::Update(float updateRate) 
{
    ///ball
    auto ballTransform = _objectTransforms[ball_Ob];
    auto P1Transform = _objectTransforms[p1_Ob];
    auto P2Transform = _objectTransforms[p2_Ob];

    //collsion with edges
    if(_mvBall.mvVector.y > .0f && ballTransform.translation.y + ballTransform.scaling.y > 1.f)        
        _mvBall.mvVector = glm::reflect(_mvBall.mvVector, {0.f, -1.f});
    else if(_mvBall.mvVector.y < .0f && ballTransform.translation.y - ballTransform.scaling.y < -1.f)
        _mvBall.mvVector = glm::reflect(_mvBall.mvVector, {0.f, 1.f});
  
    if(_mvBall.mvVector.x > .0f && ballTransform.translation.x + ballTransform.scaling.x > 1.f)
        _mvBall.mvVector = glm::reflect(_mvBall.mvVector, {-1.f, 0.f});
    else if(_mvBall.mvVector.x < .0f && ballTransform.translation.x - ballTransform.scaling.x < -1.f)
        _mvBall.mvVector = glm::reflect(_mvBall.mvVector, {1.f, 0.f});
   
    //cols. with platformss

    glm::vec2 P1_00 = {P1Transform.translation.x - P1Transform.scaling.x, P1Transform.translation.y + P1Transform.scaling.y};
    glm::vec2 P1_11 = {P1Transform.translation.x + P1Transform.scaling.x, P1Transform.translation.y - P1Transform.scaling.y};
    collide(P1_00, P1_11, ballTransform.translation, _mvBall.mvVector * ballTransform.scaling, _mvBall.mvVector);

    glm::vec2 P2_00 = {P2Transform.translation.x - P2Transform.scaling.x, P2Transform.translation.y + P2Transform.scaling.y};
    glm::vec2 P2_11 = {P2Transform.translation.x + P2Transform.scaling.x, P2Transform.translation.y - P2Transform.scaling.y};
    collide(P2_00, P2_11, ballTransform.translation, _mvBall.mvVector * ballTransform.scaling, _mvBall.mvVector);

    ///platforms

    //P1
    if(_mvP1.mvVector.y == -1.f && P1_11.y > -1.f)
        _objectTransforms[p1_Ob].translation += updateRate * _mvP1.move();
    else if(_mvP1.mvVector.y == 1.f && P1_00.y < 1.f)
        _objectTransforms[p1_Ob].translation += updateRate * _mvP1.move();

    //P2
    if(_mvP2.mvVector.y == -1.f && P2_11.y > -1.f)
        _objectTransforms[p2_Ob].translation += updateRate * _mvP2.move();
    else if(_mvP2.mvVector.y == 1.f && P2_00.y < 1.f)
        _objectTransforms[p2_Ob].translation += updateRate * _mvP2.move();    

    _objectTransforms[ball_Ob].translation += updateRate * _mvBall.move();
}
