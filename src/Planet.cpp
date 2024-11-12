#include "Planet.hpp"
#include "Physics.hpp"
#include <vector>

Planet::Planet(std::unique_ptr<Collider> collider, Vector2D pos, Vector2D vel, double mass, double radius)
    : radius(radius), Entity(std::move(collider), pos, vel, mass) { 
} 

void Planet::update(double& xGravityForce, double& yGravityForce, double& deltaTime){
    acceleration.x = Physics::acceleration(xGravityForce, mass);
    acceleration.y = Physics::acceleration(yGravityForce, mass);

    position.x += Physics::distance(velocity.x, acceleration.x, deltaTime); 
    position.y += Physics::distance(velocity.y, acceleration.y, deltaTime); 

    velocity.x += Physics::velocity(acceleration.x, deltaTime); 
    velocity.y += Physics::velocity(acceleration.y, deltaTime); 
    getCollider()->updatePosition(getPosition());
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