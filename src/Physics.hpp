#ifndef PHYSICS_HPP
#define PHYSICS_HPP

class Physics
{
public:
    // Constants
    static const double GRAVITY_CONSTANT;
    static const double EARTH_MASS;

    // Physics methods
    static double distance(double velocity, double acceleration, double timeDelta);
    static double velocity(double acceleration, double timeDelta);
    static double acceleration(double totalForce, double massEntity);
    static double gravityPull(double massPlanet, double massEntity, double distance);
    static double forceVectorYAxis(double totalForce, double angle); 
    static double forceVectorXAxis(double totalForce, double angle);

    // Getters for constants
    static double getEarthMass();
    static double getGravityConstant();
};

#endif 