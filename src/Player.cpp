#include "Player.hpp"
#include "Physics.hpp"

#include <iostream>

Player::Player(std::unique_ptr<Collider> collider, SDL_Texture* texture, Vector2D pos, Vector2D vel, double mass)
    : texture(texture), rotationSpeed(0), rotation(0), Entity(std::move(collider), pos, vel, mass) { 
} 

void Player::update(double& xGravityForce, double& yGravityForce, double& deltaTime){
    rotation += Physics::rotation(rotationSpeed, deltaTime);

    double xThrustForce = Physics::forceVectorXAxis(thrust, (rotation * M_PI) / 180); 
    double yThrustForce = -Physics::forceVectorYAxis(thrust, (rotation * M_PI) / 180);

    double xTotalForce = xGravityForce + xThrustForce; 
    double yTotalForce = yGravityForce + yThrustForce; 

    acceleration.x = Physics::acceleration(xTotalForce, mass);
    acceleration.y = Physics::acceleration(yTotalForce, mass);

    position.x += Physics::distance(velocity.x, acceleration.x, deltaTime); 
    position.y += Physics::distance(velocity.y, acceleration.y, deltaTime); 

    velocity.x += Physics::velocity(acceleration.x, deltaTime); 
    velocity.y += Physics::velocity(acceleration.y, deltaTime); 
    getCollider()->updatePosition(getPosition()); 
    getCollider()->setRotation((rotation * M_PI) / 180);
} 

void Player::draw(SDL_Renderer *renderer, int screenWidth, int screenHeight, Vector2D playerPos, Vector2D scalingFactor)
{
    int scaledWidth = static_cast<int>(playerWidth * scalingFactor.x);
    int scaledHeight = static_cast<int>(playerHeight * scalingFactor.y); 

    SDL_Rect playerDestRect = {
        screenWidth / 2 - scaledWidth / 2, 
        screenHeight / 2 - scaledHeight / 2, 
        scaledWidth,
        scaledHeight}; 

    SDL_RenderCopyEx(renderer, texture, nullptr, &playerDestRect, rotation, nullptr, SDL_FLIP_NONE);
}
