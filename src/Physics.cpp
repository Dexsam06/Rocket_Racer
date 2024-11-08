
#include "Physics.hpp"
#include <cmath>

// Define the gravity constant and Earth's mass
const double Physics::GRAVITY_CONSTANT = 6.6743e-11;

double Physics::distance(double velocity, double acceleration, double timeDelta) {
    return (velocity * timeDelta) + (0.5 * acceleration * timeDelta * timeDelta); 
}

double Physics::velocity(double acceleration, double timeDelta) { 
    return acceleration * timeDelta;
}

double Physics::acceleration(double totalForce, double massEntity) {
    return totalForce / massEntity;
}

double Physics::gravityPull(double massPlanet, double massEntity, double distance) {  
    // Formula: F = G * (m1 * m2) / r^2
    return (3 * massPlanet * massEntity) / (distance * distance);
}

double Physics::forceVectorYAxis(double totalForce, double angle) {
    // Use cosine for the Y-axis force component
    return std::cos(angle) * totalForce;
}

double Physics::forceVectorXAxis(double totalForce, double angle) { 
    // Use sine for the X-axis force component 
    return std::sin(angle) * totalForce;
}

double Physics::rotation(double rotationSpeed, double deltaTime) {
    return rotationSpeed * deltaTime;
}
