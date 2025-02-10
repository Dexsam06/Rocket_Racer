#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <vector>
#include "Entity.hpp"
#include <memory>
#include <enet/enet.h>

class Player : public Entity
{
public:
    // Constructor
    Player(std::unique_ptr<Collider> collider, SDL_Texture *texture, Vector2D pos, Vector2D vel, double mass, enet_uint32 peerID);
    ~Player(){};
    void setThrust(double thrust) { this->thrust = thrust; }
    void setPlayerWidth(int playerWidth) { this->playerWidth = playerWidth; }
    void setPlayerHeight(int playerHeight) { this->playerHeight = playerHeight; }
    void setRotationSpeed(double rotationSpeed) { this->rotationSpeed += rotationSpeed; }
    void setPosition(Vector2D position) { this->position = position; }
    void setVelocity(Vector2D velocity) { this->velocity = velocity; }
    void setRotation(double rotation) { this->rotation = rotation; }
    void setPeerID(enet_uint32 peerID) {this->peerID = peerID; }

    double getRotationSpeed() const { return rotationSpeed; }
    enet_uint32 getPeerID() {return peerID; } 
    double getThrust() {return thrust; }
    double getRotation() {return rotation; }
    
    std::unique_ptr<Entity> clone() const override
    {
        return std::make_unique<Player>(
            collider->clone(), texture, position, velocity, mass, peerID
        );
    }

private:
    double thrust;
    double rotationSpeed, rotation;
    int playerWidth, playerHeight;
    enet_uint32 peerID;

    void update(double &xGravityForce, double &yGravityForce, double &deltaTime) override;
    void draw(SDL_Renderer *renderer, int screenWidth, int screenHeight, Vector2D playerPos, Vector2D scalingFactor) override;
};

#endif
