#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <vector>
#include <memory>
#include <SDL2/SDL.h>
#include <enet/enet.h>

#include "Physics.hpp"
#include "Entity.hpp"

class Player : public Entity
{
public:
    // Constructor
    Player(std::unique_ptr<Collider> collider, Vector2D pos, Vector2D vel, double mass, enet_uint32 ID);
    ~Player(){};
 
    void setPlayerWidth(int playerWidth) { this->playerWidth = playerWidth; }
    void setPlayerHeight(int playerHeight) { this->playerHeight = playerHeight; }
    void setPosition(Vector2D position) { this->position = position; }
    void setVelocity(Vector2D velocity) { this->velocity = velocity; }
    void setRotation(double rotation) { this->rotation = rotation; }
    void setLastVerifiedInput(int lastVerifiedInput) {this->lastVerifiedInput = lastVerifiedInput; }

    double getRotationSpeed() const { return rotationSpeed; }
    Vector2D& getPosition() {return position; }
    int getLastVerifiedInput() {return lastVerifiedInput; } 

    void resetClientThrustForce() {thrustForce = 0; }
    void resetClientRotationSpeed() {rotationSpeed = 0; }
    
    std::unique_ptr<Entity> clone() const override
    {
        return std::make_unique<Player>(
            collider->clone(), position, velocity, mass, ID
        );
    } 

    void applyInput(int keyCode, float duration);
    void update(double &xGravityForce, double &yGravityForce, double &deltaTime) override; 

private:
    double thrustForce;
    double rotationSpeed;
    int playerWidth, playerHeight;
    int lastVerifiedInput; 
};

#endif 
