#ifndef PLANET_HPP
#define PLANET_HPP

#include "Entity.hpp"

class Planet : public Entity {
public:
    Planet(std::unique_ptr<Collider> collider, Vector2D pos, Vector2D vel, double mass, double radius, int R, int G, int B); 
    ~Planet(){};
private:
    double radius;

    void update(double& xGravityForce, double& yGravityForce, double& deltaTime) override;
    void draw(SDL_Renderer *renderer, int screenWidth, int screenHeight, Vector2D playerPos, Vector2D scalingFactor) override; 
    struct Color {
    int R, G, B;
    };
    Color color;
};

#endif 