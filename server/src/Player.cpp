#include "../include/Player.hpp"

#include <iostream> 

Player::Player(std::unique_ptr<Collider> collider, Vector2D pos, Vector2D vel, double mass, enet_uint32 ID) 
    : rotationSpeed(0), Entity(std::move(collider), pos, vel, mass, ID) { 
} 

void Player::update(double &xGravityForce, double &yGravityForce, double &deltaTime)
{ 
    thrustForce = isThrusting ? 100000 * deltaTime : 0; 
    rotationSpeed -= isTurningLeft ? 3 * deltaTime : 0;
    rotationSpeed += isTurningRight ? 3 * deltaTime : 0;

    rotation += Physics::rotation(rotationSpeed, deltaTime); 

    double xThrustForce = Physics::forceVectorXAxis(thrustForce, (rotation * M_PI) / 180);  
    double yThrustForce = -Physics::forceVectorYAxis(thrustForce, (rotation * M_PI) / 180);

    double xTotalForce = xGravityForce + xThrustForce; 
    double yTotalForce = yGravityForce + yThrustForce;

    acceleration.x = Physics::acceleration(xTotalForce, mass); 
    acceleration.y = Physics::acceleration(yTotalForce, mass);

    position.x += Physics::distance(velocity.x, acceleration.x, deltaTime); 
    position.y += Physics::distance(velocity.y, acceleration.y, deltaTime); 

    velocity.x += Physics::velocity(acceleration.x, deltaTime); 
    velocity.y += Physics::velocity(acceleration.y, deltaTime);  

    getCollider()->updatePosition(getPosition()); 
    getCollider()->updateRotation((rotation * M_PI) / 180);
} 


 