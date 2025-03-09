#ifndef PHYSICS_HPP
#define PHYSICS_HPP

class Physics
{
public:
    // Constants
    static const double GRAVITY_CONSTANT;

    // Physics methods
    static double distance(double velocity, double acceleration, double deltaTime);
    static double velocity(double acceleration, double deltaTime);
    static double acceleration(double totalForce, double massEntity);
    static double gravityPull(double massPlanet, double massEntity, double distance);
    static double forceVectorYAxis(double totalForce, double angle); 
    static double forceVectorXAxis(double totalForce, double angle);
    static double rotation(double rotationSpeed, double deltaTime);
    static float lerp(double a, double b, double t);
};

#endif 