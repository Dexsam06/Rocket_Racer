#ifndef PLANET_HPP
#define PLANET_HPP

#include "Entity.hpp"

class Planet : public Entity {
public:
    Planet(double xPos, double yPos, double radius, double mass, double xVelocity, double yVelocity); 
    ~Planet(){};
private:
    double radius;

    void calculatePhysics(std::vector<std::vector<double>>& entityData, double& deltaTime) override;
    void draw(SDL_Renderer *renderer, int screenWidth, int screenHeight) override; 
};

#endif 