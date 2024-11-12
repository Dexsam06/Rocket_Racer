#ifndef PLANET_HPP
#define PLANET_HPP

#include "Entity.hpp"

class Planet : public Entity {
public:
    Planet(std::unique_ptr<Collider> collider, Vector2D pos, Vector2D vel, double mass, double radius); 
    ~Planet(){};
private:
    double radius;

    void update(double& xGravityForce, double& yGravityForce, double& deltaTime) override;
    void draw(SDL_Renderer *renderer, int screenWidth, int screenHeight, Vector2D playerPos) override; 
};

#endif 