#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <SDL2/SDL_render.h>
#include <iostream>
#include <vector>
#include <memory>
#include "Vector2D.hpp"
#include "Collider.hpp" 

class Entity {
public:
    Entity(std::unique_ptr<Collider> collider, Vector2D pos, Vector2D vel, double mass); 
    virtual ~Entity() {}
    virtual void draw(SDL_Renderer *renderer, int screenWidth, int screenHeight, Vector2D playerPos, Vector2D scalingFactor) {}
    virtual void update(double& xGravityForce, double& yGravityForce, double& deltaTime) {}
    bool checkCollision(const Entity& other, Vector2D& collisionNormal, double restitution);

    Vector2D getPosition() {return position; }
    Vector2D& getVelocity() {return velocity; }
    std::unique_ptr<Collider>& getCollider() {return collider; }
    double getMass() {return mass; }
protected:
    std::unique_ptr<Collider> collider;
    Vector2D position;
    Vector2D velocity;
    Vector2D acceleration;
    double mass; 
};

#endif 
