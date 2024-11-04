#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Entity.hpp"

class Player : public Entity {
public:
    Player(SDL_Texture* texture, double xPos, double yPos, double mass, double xVelocity, double yVelocity); 
    ~Player(){}
    void setThrust(double thrust) {this->thrust = thrust; }
private:
    SDL_Texture* texture; 
    double thrust;
    void calculatePhysics(double& distance, double& massPlanet, double& xPosPlanet, double& yPosPlanet);
};

#endif 