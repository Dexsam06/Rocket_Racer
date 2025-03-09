
#include "../include/PhysicsSystem.hpp"

void PhysicsSystem::predictClientPosition(std::vector<std::unique_ptr<Entity>> &entityList, double deltaTime)
{
    size_t n = entityList.size();
    std::vector<std::pair<double, double>> forces(n, {0.0, 0.0});

    for (size_t i = 0; i < n; ++i)
    {
        for (size_t j = i + 1; j < n; ++j)
        {

            if (i == j)
                continue;

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
     
    entityList[0]->update(forces[0].first, forces[0].second, deltaTime); 
    handleCollision(entityList); 
}

void PhysicsSystem::handleCollision(std::vector<std::unique_ptr<Entity>> &entityList)
{
    double restitution = 0.2;
    
    if (entityList.empty()) return;

    auto &player = *entityList[0]; 

    for (size_t j = 1; j < entityList.size(); ++j)  
    {
        Vector2D collisionNormal;
        const auto &otherEntity = *entityList[j];

        if (player.checkCollision(otherEntity, collisionNormal, restitution))
        { 
            player.getCollider()->resolveCollision(player.getVelocity(), collisionNormal, restitution);
        }
    }
}

std::vector<Vector2D> PhysicsSystem::calculateFuturePath(std::vector<std::unique_ptr<Entity>> &entityList, double deltaTime, double predictionTime)
{
    std::vector<std::unique_ptr<Entity>> tempEntities; 
    for (std::unique_ptr<Entity> &entity : entityList)
    {
        tempEntities.push_back(entity->clone());
    }

    int steps = static_cast<int>(predictionTime / deltaTime);
    std::vector<Vector2D> futurePath;

    for (int i = 0; i < steps; ++i)
    {
        //applyGravity(tempEntities, deltaTime); 
        handleCollision(tempEntities);

        if (tempEntities.size() > 0)
            futurePath.push_back(tempEntities[0]->getPosition());
        if (tempEntities.size() > 2)
            futurePath.push_back(tempEntities[2]->getPosition());
    }

    return futurePath;
}
