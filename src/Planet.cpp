#include "Planet.hpp"
#include "Physics.hpp"
#include <vector>

Planet::Planet(Vector2D pos, Vector2D vel, double mass, double radius)
    : radius(radius), Entity(pos, vel, mass) { 
} 

void Planet::calculatePhysics(std::vector<std::vector<double>>& entityData, double& deltaTime){

    double xGravityForce = 0; 
    double yGravityForce = 0; 

    for (auto& entity : entityData) {
        double distance = sqrt(pow(entity[1] - position.x, 2) + pow(entity[2] - position.y, 2)); 
        double gravitationForce = Physics::gravityPull(entity[0], mass, distance);

        double angleOfRotationAroundEntity = atan2(entity[2] - position.y, entity[1] - position.x) - M_PI / 2;
        
        xGravityForce -= Physics::forceVectorXAxis(gravitationForce, angleOfRotationAroundEntity); 
        yGravityForce += Physics::forceVectorYAxis(gravitationForce, angleOfRotationAroundEntity);  
    }

    double xTotalForce = xGravityForce;
    double yTotalForce = yGravityForce;

    acceleration.x = Physics::acceleration(xTotalForce, mass);
    acceleration.y = Physics::acceleration(yTotalForce, mass);

    position.x += Physics::distance(velocity.x, acceleration.x, deltaTime); 
    position.y += Physics::distance(velocity.y, acceleration.y, deltaTime); 

    velocity.x += Physics::velocity(acceleration.x, deltaTime); 
    velocity.y += Physics::velocity(acceleration.y, deltaTime);
}

void Planet::draw(SDL_Renderer* renderer, int screenWidth, int screenHeight, Vector2D playerPos) {

    int centerX = position.x - playerPos.x + screenWidth / 2;
    int centerY = position.y - playerPos.y + screenHeight / 2;

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