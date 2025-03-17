#include "../include/Planet.hpp"
#include <vector> 

Planet::Planet(std::unique_ptr<Collider> collider, Vector2D pos, Vector2D vel, double mass, double radius, int ID) 
    : Entity(std::move(collider), pos, vel, mass, ID), radius(radius) {
}

void Planet::update(double& xGravityForce, double& yGravityForce, double& deltaTime){ 
    acceleration.x = Physics::acceleration(xGravityForce, mass);
    acceleration.y = Physics::acceleration(yGravityForce, mass);

    position.x += Physics::distance(velocity.x, acceleration.x, deltaTime); 
    position.y += Physics::distance(velocity.y, acceleration.y, deltaTime); 

    velocity.x += Physics::velocity(acceleration.x, deltaTime); 
    velocity.y += Physics::velocity(acceleration.y, deltaTime); 
    getCollider()->UpdatePosition(getPosition());  
}  
