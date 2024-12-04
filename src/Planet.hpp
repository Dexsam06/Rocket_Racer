#ifndef PLANET_HPP
#define PLANET_HPP

#include "Entity.hpp"

class Planet : public Entity {
public:
    Planet(std::unique_ptr<Collider> collider, SDL_Texture *texture, Vector2D pos, Vector2D vel, double mass, double radius);
    ~Planet(){};
    double getRadius() const { return radius; }
    void setRadius(double radius) { this->radius = radius; } 

    std::unique_ptr<Entity> clone() const override
    {
        return std::make_unique<Planet>(
            collider->clone(), texture, position, velocity, mass, radius 
        );
    }

private:
    double radius;
    void update(double &xGravityForce, double &yGravityForce, double &deltaTime) override;
    void draw(SDL_Renderer *renderer, int screenWidth, int screenHeight, Vector2D playerPos, Vector2D scalingFactor) override;
};

#endif
