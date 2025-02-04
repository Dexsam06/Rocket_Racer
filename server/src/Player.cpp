#include "Player.hpp"
#include "Physics.hpp"

#include <iostream> 

Player::Player(std::unique_ptr<Collider> collider, Vector2D pos, Vector2D vel, double mass, std::string username, enet_uint32 peerID) 
    : rotationSpeed(0), rotation(0), Entity(std::move(collider), pos, vel, mass), username(username), peerID(peerID) { 
} 

void Player::update(double& xGravityForce, double& yGravityForce, double& deltaTime){
    rotation += Physics::rotation(rotationSpeed, deltaTime);

    double xThrustForce = Physics::forceVectorXAxis(thrust, (rotation * M_PI) / 180); 
    double yThrustForce = -Physics::forceVectorYAxis(thrust, (rotation * M_PI) / 180);

    double xTotalForce = xGravityForce + xThrustForce; 
    double yTotalForce = yGravityForce + yThrustForce; 

    acceleration.x = Physics::acceleration(xTotalForce, mass);
    acceleration.y = Physics::acceleration(yTotalForce, mass);

    position.x += Physics::distance(velocity.x, acceleration.x, deltaTime); 
    position.y += Physics::distance(velocity.y, acceleration.y, deltaTime); 

    velocity.x += Physics::velocity(acceleration.x, deltaTime); 
    velocity.y += Physics::velocity(acceleration.y, deltaTime); 
    getCollider()->updatePosition(getPosition()); 
    getCollider()->setRotation((rotation * M_PI) / 180);
}  
