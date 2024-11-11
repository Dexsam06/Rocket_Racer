#include "Player.hpp"
#include "Physics.hpp"

#include <iostream>

Player::Player(SDL_Texture* texture, Vector2D pos, Vector2D vel, double mass)
    : texture(texture), rotationSpeed(0), rotation(0), Entity(pos, vel, mass) { 
} 

void Player::calculatePhysics(std::vector<std::vector<double>>& entityData, double& deltaTime){
    rotation += Physics::rotation(rotationSpeed, deltaTime);

    double xGravityForce = 0; 
    double yGravityForce = 0; 

    for (auto& entity : entityData) {
        double distance = sqrt(pow(entity[1] - position.x, 2) + pow(entity[2] - position.y, 2)); 
        double gravitationForce = Physics::gravityPull(entity[0], mass, distance);

        double angleOfRotationAroundEntity = atan2(entity[2] - position.y, entity[1] - position.x) - M_PI / 2;
        
        xGravityForce -= Physics::forceVectorXAxis(gravitationForce, angleOfRotationAroundEntity); 
        yGravityForce += Physics::forceVectorYAxis(gravitationForce, angleOfRotationAroundEntity);  
    }

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
    std::cout << "accelerationX: " << acceleration.x << "accelerationY: " << acceleration.y << std::endl;
}

void Player::draw (SDL_Renderer *renderer, int screenWidth, int screenHeight, Vector2D playerPos) 
{
    SDL_Rect playerDestRect = { screenWidth / 2 - playerWidth / 2, screenHeight / 2 - playerHeight / 2, playerWidth, playerHeight};
    SDL_RenderCopyEx(renderer, texture, nullptr, &playerDestRect, rotation, nullptr, SDL_FLIP_NONE);
}