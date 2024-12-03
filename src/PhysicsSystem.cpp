
#include "PhysicsSystem.hpp"

std::vector<Vector2D> PhysicsSystem::update(std::vector<std::unique_ptr<Entity>> &entityList, double deltaTime)
{
    applyGravity(entityList, deltaTime);
    handleCollision(entityList);
    return calculateFuturePath(entityList, deltaTime, 10);
}

void PhysicsSystem::applyGravity(std::vector<std::unique_ptr<Entity>> &entityList, double deltaTime)
{
    size_t n = entityList.size();
    std::vector<std::pair<double, double>> forces(n, {0.0, 0.0});

    for (size_t i = 0; i < n; ++i)
    {
        for (size_t j = i + 1; j < n; ++j)
        {
            // Use local copies to avoid mutating const getters
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

            // Use const references to avoid modifying objects when not necessary
            const auto& entity1 = *entityList[i];
            const auto& entity2 = *entityList[j];

            if (entity1.checkCollision(entity2, collisionNormal, restitution))
            {
                // Pass velocity by reference where modifications are intended
                entity1.getCollider()->resolveCollision(entityList[i]->getVelocity(), collisionNormal, restitution); 
                entity2.getCollider()->resolveCollision(entityList[j]->getVelocity(), -collisionNormal, restitution);
            }
        }
    }
}


std::vector<Vector2D> PhysicsSystem::calculateFuturePath(std::vector<std::unique_ptr<Entity>> &entityList, double deltaTime, double predictionTime)
{
    std::vector<std::unique_ptr<Entity>> tempEntities;
    for (const auto &entity : entityList)
    {
        tempEntities.push_back(entity->clone());
    }
    
    int steps = static_cast<int>(predictionTime / deltaTime);
    std::vector<Vector2D> futurePath;

    for (int i = 0; i < steps; ++i)
    {
        applyGravity(tempEntities, deltaTime);
        handleCollision(tempEntities);

        // Store the position without modifying it
        futurePath.push_back(tempEntities[0]->getPosition());
    }

    return futurePath;
}
