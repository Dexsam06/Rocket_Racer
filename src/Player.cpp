#include "Player.hpp"
#include "Physics.hpp"

Player::Player(SDL_Texture* texture, double xPos, double yPos, double mass, double xVelocity, double yVelocity)
    : texture(texture), Entity(xPos, yPos, mass, xVelocity, yVelocity) { 
} 

void Player::calculatePhysics(double& distance, double& massPlanet, double xPosPlanet, double yPosPlanet){
    double gravitationForce = Physics::gravityPull(massPlanet, mass, distance); 
    double angleOfRotationAroundPlanet = atan2(yPosPlanet - yPos, xPosPlanet - xPos) - M_PI / 2; 
    double xForceAxis = Physics::forceVectorXAxis(gravitationForce, angleOfRotationAroundPlanet);
    double yForceAxis = Physics::forceVectorYAxis(gravitationForce, angleOfRotationAroundPlanet);
    


    acceleration = acceleration()
}