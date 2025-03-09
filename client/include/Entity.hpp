#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <SDL2/SDL_render.h>
#include <iostream>
#include <vector>
#include <memory>
#include "Vector2D.hpp"
#include "Collider.hpp"
#include <enet/enet.h>

class Entity  
{
public:
    Entity(std::unique_ptr<Collider> collider, SDL_Texture *texture, Vector2D pos, Vector2D vel, double mass, enet_uint32 ID); 

    virtual std::unique_ptr<Entity> clone() const { return std::make_unique<Entity>(collider->clone(), texture, position, velocity, mass, ID);}

    virtual void draw(SDL_Renderer *renderer, int screenWidth, int screenHeight, Vector2D playerClientPos, Vector2D scalingFactor) {}
    virtual void update(double &xGravityForce, double &yGravityForce, double &deltaTime) {} 

    bool checkCollision(const Entity &other, Vector2D &collisionNormal, double restitution) const;

    Vector2D getPosition() const { return position; }
    Vector2D &getVelocity() { return velocity; }
    const Vector2D &getVelocity() const { return velocity; }
    const std::unique_ptr<Collider> &getCollider() const { return collider; }
    double getMass() const { return mass; }
    Vector2D getAcceleration() const { return acceleration; }
    enet_uint32 getID() {return ID; } 
    SDL_Texture* getTexture() {return texture; }
    double getRotation() {return rotation; }

    void setPosition(Vector2D position) { this->position = position; }
    void setVelocity(Vector2D velocity) { this->velocity = velocity; }
    void setAcceleration(Vector2D acceleration) { this->acceleration = acceleration; } 
    void setMass(double mass) { this->mass = mass; }
    void setID(enet_uint32 ID) {this->ID = ID; }
    

protected:
    std::unique_ptr<Collider> collider;
    SDL_Texture *texture;
    Vector2D position;
    Vector2D velocity;
    Vector2D acceleration;
    double mass;
    enet_uint32 ID;
    double rotation;
};

#endif
