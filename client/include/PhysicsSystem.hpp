
#ifndef PHYSICSSYSTEM_HPP
#define PHYSICSSYSTEM_HPP

#include <vector>
#include "Entity.hpp"
#include "Vector2D.hpp"
#include "Physics.hpp"
#include "Collider.hpp"
#include "Player.hpp"

class PhysicsSystem 
{
public:
    void predictClientPosition(std::vector<std::unique_ptr<Entity>> &entityList, double deltaTime);
private: 
    void handleCollision(std::vector<std::unique_ptr<Entity>> &entityList); 
};

#endif  