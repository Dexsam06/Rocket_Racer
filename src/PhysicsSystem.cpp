
#include "PhysicsSystem.hpp"

void PhysicsSystem::update(std::vector<Entity *> &entityList, double deltaTime){
    applyGravity(entityList, deltaTime); 
    handleCollision(entityList);
}

void PhysicsSystem::applyGravity(std::vector<Entity*>& entityList, double deltaTime) {
    size_t n = entityList.size();
    std::vector<std::pair<double, double>> forces(n, {0.0, 0.0}); 

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            if (i == j) continue;
            
            double dx = entityList[j]->getPosition().x - entityList[i]->getPosition().x;
            double dy = entityList[j]->getPosition().y - entityList[i]->getPosition().y;
            double distance = std::sqrt(dx * dx + dy * dy);
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

    for (size_t i = 0; i < n; ++i) {
        entityList[i]->update(forces[i].first, forces[i].second, deltaTime); 
    }
}

void PhysicsSystem::handleCollision(std::vector<Entity *> &entityList){ 
    double restitution = 0.2;
    for (size_t i = 0; i < entityList.size(); ++i) {
        for (size_t j = i + 1; j < entityList.size(); ++j) {  
            if(i == j) continue;
            Vector2D collisionNormal;
            if (entityList[i]->checkCollision(*entityList[j], collisionNormal, restitution)) { 
                entityList[i]->getCollider()->resolveCollision(entityList[i]->getVelocity(), collisionNormal, restitution);
                entityList[j]->getCollider()->resolveCollision(entityList[j]->getVelocity(), -collisionNormal, restitution);
            }  
        } 
    }
}   