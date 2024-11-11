#ifndef PLANET_HPP
#define PLANET_HPP

#include "Entity.hpp"

class Planet : public Entity {
public:
    Planet(Vector2D pos, Vector2D vel, double mass, double radius); 
    ~Planet(){};
private:
    double radius;

    void calculatePhysics(std::vector<std::vector<double>>& entityData, double& deltaTime) override;
    void draw(SDL_Renderer *renderer, int screenWidth, int screenHeight, Vector2D playerPos) override; 
};

#endif 