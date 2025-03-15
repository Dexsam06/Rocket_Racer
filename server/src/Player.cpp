#include "../include/Player.hpp"

#include <iostream> 

Player::Player(std::unique_ptr<Collider> collider, Vector2D pos, Vector2D vel, double mass, enet_uint32 ID) 
    : rotationSpeed(0), Entity(std::move(collider), pos, vel, mass, ID) { 
} 

void Player::applyInput(int keyCode, float duration) 
{
    switch (keyCode) 
    {
        //Left arrow
        case 1073741904: 
            rotationSpeed -= duration * 2;
            break;    
        //Right arrow
        case 1073741903:
            rotationSpeed += duration * 2; 
            break;
        //Spacebar
        case 32:
            thrustForce +=  duration * 100000; 
            break;
    }
}

void Player::update(double &xGravityForce, double &yGravityForce, double &deltaTime)
{ 
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

    thrustForce = 0; 

    getCollider()->updatePosition(getPosition()); 
    getCollider()->setRotation((rotation * M_PI) / 180);
} 


 