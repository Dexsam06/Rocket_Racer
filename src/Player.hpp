
#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <vector>
#include "Entity.hpp"


class Player : public Entity {
public:
    Player(std::unique_ptr<Collider> collider, SDL_Texture* texture, Vector2D pos, Vector2D vel, double mass); 
    ~Player(){}

    void setThrust(double thrust) {this->thrust = thrust; }
    void setPlayerWidth(int playerWidth) {this->playerWidth = playerWidth; }
    void setPlayerHeight(int playerHeight) {this->playerHeight = playerHeight; }
    void setRotationSpeed(double rotationSpeed) {this->rotationSpeed += rotationSpeed; }
private:
    SDL_Texture* texture;   
    double thrust;
    double rotationSpeed, rotation;  
    int playerWidth, playerHeight; 

    void update(double& xGravityForce, double& yGravityForce, double& deltaTime) override;
    void draw(SDL_Renderer *renderer, int screenWidth, int screenHeight, Vector2D playerPos, Vector2D scalingFactor) override; 
};  

#endif 