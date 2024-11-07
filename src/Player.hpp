#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Entity.hpp"
#include <vector>

class Player : public Entity {
public:
    Player(SDL_Texture* texture, double xPos, double yPos, double mass, double xVelocity, double yVelocity); 
    ~Player(){}

    void setThrust(double thrust) {this->thrust = thrust; }
    void setPlayerWidth(int playerWidth) {this->playerWidth = playerWidth; }
    void setPlayerHeight(int playerHeight) {this->playerHeight = playerHeight; }
    void setRotationSpeed(double rotationSpeed) {this->rotationSpeed += rotationSpeed; }
    double getPlayerXPos() {return xPos; }
    double getPlayerYPos() {return yPos; }
    double getPlayerXVelocity() {return xVelocity; }
    double getPlayerYVelocity() {return yVelocity; }
private:
    SDL_Texture* texture;   
    double thrust;
    double rotationSpeed, rotation;  
    int playerWidth, playerHeight; 

    void calculatePhysics(std::vector<std::vector<double>>& entityData, double& deltaTime) override;
    void draw(SDL_Renderer *renderer, int screenWidth, int screenHeight) override; 
};  

#endif 