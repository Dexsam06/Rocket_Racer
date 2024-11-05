#ifndef PLANET_HPP
#define PLANET_HPP

#include "Entity.hpp"

class Planet : public Entity {
public:
    Planet(double radius, double xPos, double yPos, double mass, double xVelocity, double yVelocity); 
    ~Planet(){};
private:
    double radius;
    void draw(SDL_Renderer *renderer, double screenWidth, double screenHeight) override; 
};

#endif 