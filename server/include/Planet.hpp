#ifndef PLANET_HPP
#define PLANET_HPP

#include "Entity.hpp"
#include "Physics.hpp"

class Planet : public Entity {
public:
    Planet(std::unique_ptr<Collider> collider, Vector2D pos, Vector2D vel, double mass, double radius, int ID);
    ~Planet(){};

    void setRadius(double radius) { this->radius = radius; } 

    double getRadius() const { return radius; }
private:
    double radius;

    void update(double &xGravityForce, double &yGravityForce, double &deltaTime) override; 
};

#endif 
