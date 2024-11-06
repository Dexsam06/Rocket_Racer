#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Entity.hpp"
#include <vector>

class Player : public Entity {
public:
    Player(SDL_Texture* texture, double xPos, double yPos, double mass, double xVelocity, double yVelocity); 
    ~Player(){}
    void calculatePhysics(std::vector<std::vector<double>> data, double& deltaTime);

    void setThrust(double thrust) {this->thrust = thrust; }
    void setPlayerWidth(int playerWidth) {this->playerWidth = playerWidth; }
    void setPlayerHeight(int playerHeight) {this->playerHeight = playerHeight; }
private:
    SDL_Texture* texture;   
    double thrust;
    double rotationSpeed, rotation; 
    int playerWidth, playerHeight;
    void draw(SDL_Renderer *renderer, int screenWidth, int screenHeight) override; 
};  

#endif 