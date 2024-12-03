
#ifndef PLANET_HPP
#define PLANET_HPP

#include "Entity.hpp"

class Planet : public Entity {
public:
    Planet(std::unique_ptr<Collider> collider, Vector2D pos, Vector2D vel, double mass, double radius, int R, int G, int B)
        : Entity(std::move(collider), pos, vel, mass), radius(radius), color{R, G, B} {}

    // Default constructor for cloning
    Planet() : radius(0.0), color{0, 0, 0} {}

    ~Planet() override {}

    std::unique_ptr<Entity> clone() const override
    {
        auto planetClone = std::make_unique<Planet>();

        planetClone->setPosition(this->getPosition());
        planetClone->setVelocity(this->getVelocity());
        planetClone->setAcceleration(this->getAcceleration());
        planetClone->setMass(this->getMass());
        planetClone->setRadius(this->getRadius());

        if (this->collider)
        {
            planetClone->collider = this->collider->clone();
        }

        planetClone->color = this->color;
        return planetClone;
    }

    double getRadius() const { return radius; }
    void setRadius(double radius) { this->radius = radius; }

private:
    double radius;

    void update(double &xGravityForce, double &yGravityForce, double &deltaTime) override;
    void draw(SDL_Renderer *renderer, int screenWidth, int screenHeight, Vector2D playerPos, Vector2D scalingFactor) override;

    struct Color {
        int R, G, B;
    };
    Color color;
};

#endif
