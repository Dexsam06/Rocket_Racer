
#include "PhysicsSystem.hpp"

void PhysicsSystem::update(std::vector<std::unique_ptr<Entity>> &entityList, double deltaTime) 
{
    applyGravity(entityList, deltaTime);
    handleCollision(entityList);
}

void PhysicsSystem::applyGravity(std::vector<std::unique_ptr<Entity>> &entityList, double deltaTime)
{
    size_t n = entityList.size();
    std::vector<std::pair<double, double>> forces(n, {0.0, 0.0}); 

    for (size_t i = 0; i < n; ++i)
    {
        for (size_t j = i + 1; j < n; ++j) 
        {
            const Vector2D pos1 = entityList[i]->getPosition();
            const Vector2D pos2 = entityList[j]->getPosition();

            double dx = pos2.x - pos1.x;
            double dy = pos2.y - pos1.y;
            double epsilon = 1e-3;
            double distance = std::sqrt(dx * dx + dy * dy) + epsilon;
            double force = Physics::gravityPull(entityList[i]->getMass(), entityList[j]->getMass(), distance);

            double angle = std::atan2(dy, dx);
            double fx = force * std::cos(angle);
            double fy = force * std::sin(angle);

            forces[i].first += fx;
            forces[i].second += fy;
            forces[j].first -= fx;
            forces[j].second -= fy;
        }
    }

    for (size_t i = 0; i < n; ++i)
    {
        entityList[i]->update(forces[i].first, forces[i].second, deltaTime);
    }
}

void PhysicsSystem::handleCollision(std::vector<std::unique_ptr<Entity>> &entityList)
{
    double restitution = 0.2;
    for (size_t i = 0; i < entityList.size(); ++i)
    {
        for (size_t j = i + 1; j < entityList.size(); ++j) 
        {
            if (i == j) 
                continue; 
            
            Vector2D collisionNormal; 

            
            const auto& entity1 = *entityList[i]; 
            const auto& entity2 = *entityList[j];

            if (entity1.checkCollision(entity2, collisionNormal, restitution)) 
            {
                entity1.getCollider()->resolveCollision(entityList[i]->getVelocity(), collisionNormal, restitution); 
                entity2.getCollider()->resolveCollision(entityList[j]->getVelocity(), -collisionNormal, restitution);
            }
        }
    }
}
