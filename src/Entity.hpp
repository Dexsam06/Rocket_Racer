#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <SDL2/SDL_render.h>
#include <vector>
#include "Vector2D.hpp"

class Entity {
public:
    Entity(Vector2D pos, Vector2D vel, double mass); 
    virtual ~Entity() {}
    virtual void draw(SDL_Renderer *renderer, int screenWidth, int screenHeight, Vector2D playerPos) {}
    virtual void calculatePhysics(std::vector<std::vector<double>>& entityData, double& deltaTime) {}

    Vector2D getPosition() {return position; }
    Vector2D getVelocity() {return velocity; }
    double getMass() {return mass; }
protected:
    Vector2D position;
    Vector2D velocity;
    Vector2D acceleration;
    double mass; 
};

#endif 
