
#ifndef PHYSICSSYSTEM_HPP
#define PHYSICSSYSTEM_HPP

#include <vector>
#include "Entity.hpp"
#include "Vector2D.hpp"

class PhysicsSystem
{
public:
    void update(std::vector<Entity *> &entityList, double deltaTime);
private:
    void applyGravity(std::vector<Entity *> &entityList, double deltaTime);
    void handleCollision(); 
};

#endif