#include "../include/Player.hpp"
#include "Physics.hpp"

#include <iostream> 

Player::Player(std::unique_ptr<Collider> collider, SDL_Texture* texture, Vector2D pos, Vector2D vel, double mass, enet_uint32 ID) 
    : rotationSpeed(0), Entity(std::move(collider), texture, pos, vel, mass, ID) { 
} 

void Player::update(double& xGravityForce, double& yGravityForce, double& deltaTime){ 
    rotation += Physics::rotation(rotationSpeed, deltaTime); 

    double xThrustForce = Physics::forceVectorXAxis(thrustForce, (rotation * M_PI) / 180);  
    double yThrustForce = -Physics::forceVectorYAxis(thrustForce, (rotation * M_PI) / 180);

    double xTotalForce = xGravityForce + xThrustForce; 
    double yTotalForce = yGravityForce + yThrustForce;

    acceleration.x = Physics::acceleration(xTotalForce, mass); 
    acceleration.y = Physics::acceleration(yTotalForce, mass);

    position.x += Physics::distance(velocity.x, acceleration.x, deltaTime); 
    position.y += Physics::distance(velocity.y, acceleration.y, deltaTime); 

    velocity.x += Physics::velocity(acceleration.x, deltaTime); 
    velocity.y += Physics::velocity(acceleration.y, deltaTime);  
}  

void Player::applyInput(int keyCode, float duration)
{
    switch (keyCode) 
    {
        //Left arrow
        case 8592:
            rotationSpeed -= duration * 0.1;
            break;    
        //Right arrow
        case 8594:
            rotationSpeed += duration * 0.1; 
            break;
        //Spacebar
        case 32:
            thrustForce +=  duration * 100; 
            break;
    }
}

void Player::draw(SDL_Renderer *renderer, int screenWidth, int screenHeight, Vector2D playerClientPos, Vector2D scalingFactor) 
{
    Vector2D screenCenter(screenWidth / 2, screenHeight / 2); 

    Vector2D offsetFromClientPlayer(
        position.x - playerClientPos.x,  
        position.y - playerClientPos.y
    );

    Vector2D scaledOffset(
        offsetFromClientPlayer.x * scalingFactor.x, 
        offsetFromClientPlayer.y * scalingFactor.y 
    );

    Vector2D scaledPosition( 
        screenCenter.x + scaledOffset.x, 
        screenCenter.y + scaledOffset.y 
    );

    int scaledWidth = static_cast<int>(playerWidth * scalingFactor.x);
    int scaledHeight = static_cast<int>(playerHeight * scalingFactor.y);  

    SDL_Rect playerDestRect = { 
        screenWidth / 2 - scaledWidth / 2, 
        screenHeight / 2 - scaledHeight / 2,  
        scaledWidth,
        scaledHeight}; 

    SDL_RenderCopyEx(renderer, texture, nullptr, &playerDestRect, rotation, nullptr, SDL_FLIP_NONE);
} 

void Player::reconcileClientState(Vector2D &clientPos, Vector2D serverPos, double serverRotation, double lerpFactor) {
    double deltaX = serverPos.x - clientPos.x;
    double deltaY = serverPos.y - clientPos.y;
    double distanceSquared = (deltaX * deltaX) + (deltaY * deltaY);

    double snapThreshold = 0.5;

    // Check if we should snap (if within the threshold)
    if (distanceSquared < snapThreshold * snapThreshold) {
        clientPos.x = serverPos.x; 
        clientPos.y = serverPos.y;
    } else {
        // Smoothly interpolate towards the correct position
        clientPos.x = Physics::lerp(clientPos.x, serverPos.x, lerpFactor);
        clientPos.y = Physics::lerp(clientPos.y, serverPos.y, lerpFactor);
    }

    if(rotation > serverRotation - 5 && rotation < serverRotation + 5)
    {
        rotation = serverRotation;
    } else {
        rotation = Physics::lerp(rotation, serverRotation, lerpFactor); 
    }

    thrustForce = 0.0; 
    rotationSpeed = 0.0; 
}
 