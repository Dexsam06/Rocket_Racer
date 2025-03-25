#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <vector>
#include <memory>
#include <SDL2/SDL.h>
#include <enet/enet.h>

#include "Physics.hpp"
#include "Entity.hpp"
#include "NetworkPackets.hpp"

class Player : public Entity
{
public:
    // Constructor
    Player(std::unique_ptr<Collider> collider, SDL_Texture *texture, Vector2D pos, Vector2D vel, double mass, enet_uint32 ID, std::string username);
    ~Player(){};
 
    void setPlayerWidth(int playerWidth) { this->playerWidth = playerWidth; }
    void setPlayerHeight(int playerHeight) { this->playerHeight = playerHeight; }
    void setPosition(Vector2D position) { this->position = position; }
    void setVelocity(Vector2D velocity) { this->velocity = velocity; }
    void setRotation(double rotation) { this->rotation = rotation; }

    double getRotationSpeed() const { return rotationSpeed; }
    Vector2D& getPosition() {return position; }
    int getPlayerWidth() {return playerWidth; }
    int getPlayerHeight() {return playerHeight; }
    std::string getUsername() {return username; } 
    bool getisThrusting() {return isThrusting; }

    void update(double &xGravityForce, double &yGravityForce, double &deltaTime) override;
    void reconcileClientState(GameStatePacket &gameStatePacket, double lerpFactor); 
    void applyInput(SDL_Keycode keyCode, bool pressed); 

private:
    bool isThrusting = false, isTurningLeft = false, isTurningRight = false; 
    double rotationSpeed; 
    double thrustForce; 
    int playerWidth, playerHeight;
    std::string username; 

    void draw(SDL_Renderer *renderer, int screenWidth, int screenHeight, Vector2D playerClientPos, Vector2D scalingFactor) override;
};

#endif 
