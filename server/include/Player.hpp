#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <vector>
#include <memory>
#include <SDL2/SDL.h>
#include <enet/enet.h>
#include <unordered_map>

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
    
    void setisThrusting(bool state) {isThrusting = state; }
    void setisTurningLeft(bool state) {isTurningLeft = state; }
    void setisTurningRight(bool state) {isTurningRight = state; }

    double getRotationSpeed() const { return rotationSpeed; }
    Vector2D& getPosition() {return position; }

    bool getisThrusting() {return isThrusting; }
    bool getisTurningLeft() {return isTurningLeft; }
    bool getisTurningRight() {return isTurningRight; }

    void resetClientThrustForce() {thrustForce = 0; }
    void resetClientRotationSpeed() {rotationSpeed = 0; }

    void update(double &xGravityForce, double &yGravityForce, double &deltaTime) override; 

private:
    bool isThrusting = false, isTurningLeft = false, isTurningRight = false; 
    double thrustForce; 
    double rotationSpeed;
    int playerWidth, playerHeight;  
}; 

#endif 
