#include "Player.hpp"
#include "Physics.hpp"

#include <iostream>

Player::Player(SDL_Texture* texture, double xPos, double yPos, double mass, double xVelocity, double yVelocity)
    : texture(texture), rotationSpeed(0), rotation(0), Entity(xPos, yPos, mass, xVelocity, yVelocity) { 
} 

void Player::calculatePhysics(std::vector<std::vector<double>>& entityData, double& deltaTime){
    rotation += Physics::rotation(rotationSpeed, deltaTime);

    double xGravityForce = 0; 
    double yGravityForce = 0; 

    for (auto& entity : entityData) {
        double distance = sqrt(pow(entity[1] - xPos, 2) + pow(entity[2] - yPos, 2)); 
        double gravitationForce = Physics::gravityPull(entity[0], mass, distance);

        double angleOfRotationAroundEntity = atan2(entity[2] - yPos, entity[1] - xPos) - M_PI / 2;
        
        xGravityForce -= Physics::forceVectorXAxis(gravitationForce, angleOfRotationAroundEntity); 
        yGravityForce += Physics::forceVectorYAxis(gravitationForce, angleOfRotationAroundEntity); 
    }

    double xThrustForce = Physics::forceVectorXAxis(thrust, (rotation * M_PI) / 180); 
    double yThrustForce = -Physics::forceVectorYAxis(thrust, (rotation * M_PI) / 180);

    double xTotalForce = xGravityForce + xThrustForce;
    double yTotalForce = yGravityForce + yThrustForce;

    xAcceleration = Physics::acceleration(xTotalForce, mass);
    yAcceleration = Physics::acceleration(yTotalForce, mass);

    xPos += Physics::distance(xVelocity, xAcceleration, deltaTime);
    yPos += Physics::distance(yVelocity, yAcceleration, deltaTime); 

    xVelocity += Physics::velocity(xAcceleration, deltaTime);
    yVelocity += Physics::velocity(yAcceleration, deltaTime);

    std::cout << "xPosPlayer: " << xPos << std::endl;
    std::cout << "yPosPlayer: " << yPos << std::endl;
}

void Player::draw (SDL_Renderer *renderer, int screenWidth, int screenHeight) 
{
    SDL_Rect playerDestRect = { screenWidth / 2 - playerWidth / 2, screenHeight / 2 - playerHeight / 2, playerWidth, playerHeight};
    SDL_RenderCopyEx(renderer, texture, nullptr, &playerDestRect, rotation, nullptr, SDL_FLIP_NONE);
}