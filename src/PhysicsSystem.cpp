
#include "PhysicsSystem.hpp"

void PhysicsSystem::update(std::vector<Entity *> &entityList, double deltaTime){
    applyGravity(entityList, deltaTime); 
}

void PhysicsSystem::applyGravity(std::vector<Entity *> &entityList, double deltaTime){ 

    std::vector<std::vector<double>> entityData;
    for (const auto &entity : entityList)
    {
        entityData.push_back({entity->getMass(), entity->getPosition().x, entity->getPosition().y}); 
    }

    for (size_t i = 0; i < entityList.size(); ++i)
    {
        std::vector<std::vector<double>> entityDataFinal; 

        for (size_t j = 0; j < entityList.size(); ++j) 
        {
            if (i == j) 
                continue; 
            entityDataFinal.push_back(entityData[j]);
        }

        entityList[i]->calculatePhysics(entityDataFinal, deltaTime);
    }
}

void PhysicsSystem::handleCollision(){}