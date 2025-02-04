#include "Planet.hpp"
#include "Physics.hpp"
#include <vector> 

Planet::Planet(std::unique_ptr<Collider> collider, SDL_Texture *texture, Vector2D pos, Vector2D vel, double mass, double radius) 
    : Entity(std::move(collider), texture, pos, vel, mass), radius(radius) {
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

    int scaledWidth = static_cast<int>((2 * radius) * scalingFactor.x);
    int scaledHeight = static_cast<int>((2 * radius) * scalingFactor.y); 

    SDL_Rect planetDestRect = {
        scaledPosition.x - scaledWidth / 2, 
        scaledPosition.y - scaledHeight /2, 
        scaledWidth,
        scaledHeight}; 

    SDL_RenderCopy(renderer, texture, nullptr, &planetDestRect);
}
