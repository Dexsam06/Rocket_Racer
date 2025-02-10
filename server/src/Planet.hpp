#ifndef PLANET_HPP
#define PLANET_HPP

#include "Entity.hpp"

class Planet : public Entity {
public:
    Planet(std::unique_ptr<Collider> collider, Vector2D pos, Vector2D vel, double mass, double radius, int uniqueID);
    ~Planet(){};

    void setRadius(double radius) { this->radius = radius; } 

    int getUniqueID() {return uniqueID; }
    double getRadius() const { return radius; }

    std::unique_ptr<Entity> clone() const override
    {
        return std::make_unique<Planet>(
            collider->clone(), position, velocity, mass, radius, uniqueID
        );
    }

private:
    double radius;
    int uniqueID;

    void update(double &xGravityForce, double &yGravityForce, double &deltaTime) override; 
};

#endif
