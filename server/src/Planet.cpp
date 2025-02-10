#include "Planet.hpp"
#include "Physics.hpp"
#include <vector> 

Planet::Planet(std::unique_ptr<Collider> collider, Vector2D pos, Vector2D vel, double mass, double radius, int uniqueID) 
    : Entity(std::move(collider), pos, vel, mass), radius(radius), uniqueID(uniqueID) {
}

void Planet::update(double& xGravityForce, double& yGravityForce, double& deltaTime){
    acceleration.x = Physics::acceleration(xGravityForce, mass);
    acceleration.y = Physics::acceleration(yGravityForce, mass);

    position.x += Physics::distance(velocity.x, acceleration.x, deltaTime); 
    position.y += Physics::distance(velocity.y, acceleration.y, deltaTime); 

    velocity.x += Physics::velocity(acceleration.x, deltaTime); 
    velocity.y += Physics::velocity(acceleration.y, deltaTime); 
    getCollider()->updatePosition(getPosition());
}  
