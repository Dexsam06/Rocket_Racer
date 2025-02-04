
#ifndef PHYSICSSYSTEM_HPP
#define PHYSICSSYSTEM_HPP

#include <vector>
#include "Entity.hpp"
#include "Vector2D.hpp"
#include "Physics.hpp"
#include "Collider.hpp"

class PhysicsSystem 
{
public:
    std::vector<Vector2D> update(std::vector<std::unique_ptr<Entity>> &entityList, double deltaTime);
private:
    void applyGravity(std::vector<std::unique_ptr<Entity>> &entityList, double deltaTime);
    void handleCollision(std::vector<std::unique_ptr<Entity>> &entityList); 
    std::vector<Vector2D> calculateFuturePath(std::vector<std::unique_ptr<Entity>> &entityList, double deltaTime, double predictionTime);
};

#endif