#include "Player.hpp"
#include "Physics.hpp"

Player::Player(SDL_Texture* texture, double xPos, double yPos, double mass, double xVelocity, double yVelocity)
    : texture(texture), Entity(xPos, yPos, mass, xVelocity, yVelocity) { 
} 

void Player::calculatePhysics(std::vector<std::vector<double>> data, double& deltaTime){

    double xForceAxis;
    double yForceAxis;
    double gravitationForce;
    double angleOfRotationAroundPlanet;

    for (const auto& row : data) {
        double distance = sqrt(std::pow(row[1] - xPos, 2) + pow(row[2] - yPos, 2));
        gravitationForce = Physics::gravityPull(row[0], mass, distance); 
        angleOfRotationAroundPlanet = atan2(row[2] - yPos, row[1] - xPos); 
        xForceAxis += Physics::forceVectorXAxis(gravitationForce, angleOfRotationAroundPlanet); 
        yForceAxis += Physics::forceVectorYAxis(gravitationForce, angleOfRotationAroundPlanet); 
    }

    xForceAxis += Physics::forceVectorXAxis(thrust, rotation);
    yForceAxis += Physics::forceVectorYAxis(thrust, rotation);

    xAcceleration = Physics::acceleration(xForceAxis, mass);
    yAcceleration = Physics::acceleration(yForceAxis, mass);

    xPos += Physics::distance(xVelocity, xAcceleration, deltaTime);
    yPos -= Physics::distance(yVelocity, yAcceleration, deltaTime);

    xVelocity += Physics::velocity(xAcceleration, deltaTime);
    yVelocity += Physics::velocity(yAcceleration, deltaTime);
}

void Player::draw (SDL_Renderer *renderer, int screenWidth, int screenHeight) 
{
    SDL_Rect playerDestRect = { screenWidth / 2 - playerWidth / 2, screenHeight / 2 - playerHeight / 2, playerWidth, playerHeight };
    SDL_RenderCopyEx(renderer, texture, nullptr, &playerDestRect, rotation, nullptr, SDL_FLIP_NONE);
}