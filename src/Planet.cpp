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

void Planet::draw(SDL_Renderer *renderer, int screenWidth, int screenHeight, Vector2D playerPos, Vector2D scalingFactor)  
{
    Vector2D screenCenter(screenWidth / 2, screenHeight / 2);

    Vector2D offsetFromPlayer(
        position.x - playerPos.x, 
        position.y - playerPos.y
    );

    Vector2D scaledOffset(
        offsetFromPlayer.x * scalingFactor.x, 
        offsetFromPlayer.y * scalingFactor.y 
    );

    Vector2D scaledPosition(
        screenCenter.x + scaledOffset.x,
        screenCenter.y + scaledOffset.y
    );

    int scaledRadius = static_cast<int>(radius * (scalingFactor.x + scalingFactor.y) / 2); 

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    for (int dy = -scaledRadius; dy <= scaledRadius; ++dy)
    {
        for (int dx = -scaledRadius; dx <= scaledRadius; ++dx)
        {
            if (dx * dx + dy * dy <= scaledRadius * scaledRadius)
            {
                SDL_RenderDrawPoint(renderer, static_cast<int>(scaledPosition.x + dx), static_cast<int>(scaledPosition.y + dy));  
            } 
        }
    }

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); 
    SDL_RenderDrawLine(renderer, scaledPosition.x, scaledPosition.y, screenCenter.x, screenCenter.y);
}
