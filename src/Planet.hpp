#ifndef PLANET_HPP
#define PLANET_HPP

#include "Entity.hpp"

class Planet : public Entity {
public:
    Planet(std::unique_ptr<Collider> collider, Vector2D pos, Vector2D vel, double mass, double radius, int R, int G, int B);
    ~Planet(){};
    double getRadius() const { return radius; }
    void setRadius(double radius) { this->radius = radius; } 

    std::unique_ptr<Entity> clone() const override
    {
        return std::make_unique<Planet>(
            collider->clone(), position, velocity, mass, radius, color.r, color.g, color.b
        );
    }

private:
    double radius;

    void update(double &xGravityForce, double &yGravityForce, double &deltaTime) override;
    void draw(SDL_Renderer *renderer, int screenWidth, int screenHeight, Vector2D playerPos, Vector2D scalingFactor) override;
    SDL_Color color; 
};

#endif
