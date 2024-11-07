#include "Planet.hpp"
#include "Physics.hpp"
#include <vector>

Planet::Planet(double xPos, double yPos, double radius, double mass, double xVelocity, double yVelocity)
    : radius(radius), Entity(xPos, yPos, mass, xVelocity, yVelocity) { 
} 

void Planet::calculatePhysics(std::vector<std::vector<double>>& entityData, double& deltaTime){

    double xGravityForce = 0; 
    double yGravityForce = 0; 

    for (auto& entity : entityData) {
        double distance = sqrt(pow(entity[1] - xPos, 2) + pow(entity[2] - yPos, 2)); 
        double gravitationForce = Physics::gravityPull(entity[0], mass, distance);

        double angleOfRotationAroundEntity = atan2(entity[2] - yPos, entity[1] - xPos) - M_PI / 2;
        
        xGravityForce -= Physics::forceVectorXAxis(gravitationForce, angleOfRotationAroundEntity);
        yGravityForce += Physics::forceVectorYAxis(gravitationForce, angleOfRotationAroundEntity); 
    }

    xAcceleration = Physics::acceleration(xGravityForce, mass);
    yAcceleration = Physics::acceleration(yGravityForce, mass);

    xPos += Physics::distance(xVelocity, xAcceleration, deltaTime);
    yPos += Physics::distance(yVelocity, yAcceleration, deltaTime); 

    xVelocity += Physics::velocity(xAcceleration, deltaTime);
    yVelocity += Physics::velocity(yAcceleration, deltaTime);
}

void Planet::draw(SDL_Renderer* renderer, int screenWidth, int screenHeight) {
   
    int centerX = xPos - entityXPos + screenWidth / 2;
    int centerY = yPos - entityYPos + screenHeight / 2;

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); 

    for (int y = centerY - radius; y <= centerY + radius; ++y) {
        for (int x = centerX - radius; x <= centerX + radius; ++x) { 
            int dx = x - centerX;
            int dy = y - centerY;

            if (dx * dx + dy * dy <= radius * radius) { 
                SDL_RenderDrawPoint(renderer, x, y);  
            }
        }
    }
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderDrawLine(renderer, centerX, centerY, screenWidth / 2, screenHeight / 2);
}