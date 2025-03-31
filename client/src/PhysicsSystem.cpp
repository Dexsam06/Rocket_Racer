
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

    for (size_t i = 0; i < n; ++i)
    {
        entityList[i]->update(forces[i].first, forces[i].second, deltaTime);
    }

    handleCollision(entityList);  
}

void PhysicsSystem::handleCollision(std::vector<std::unique_ptr<Entity>>& entities) {
    for (size_t i = 0; i < entities.size(); ++i) {
        for (size_t j = i + 1; j < entities.size(); ++j) {
            Entity* a = entities[i].get();
            Entity* b = entities[j].get();

            if (!a->getCollider() || !b->getCollider()) continue;

            Vector2D normal;
            double penetration = 0;

            if (a->getCollider()->CheckCollision(*b->getCollider(), normal, penetration)) {
                double invMassA = a->getInverseMass();
                double invMassB = b->getInverseMass();
                double totalInvMass = invMassA + invMassB; 
                
                if (totalInvMass <= 0) continue; 

                Vector2D correction = normal * (penetration / totalInvMass);
                a->setPosition(a->getPosition() - correction * invMassA);
                b->setPosition(b->getPosition() + correction * invMassB);

                Vector2D relativeVelocity = b->getVelocity() - a->getVelocity();
                double velocityAlongNormal = relativeVelocity.dot(normal);

                // Only resolve if objects are moving toward each other
                if (velocityAlongNormal > 0) continue;

                // Apply velocity correction
                double restitution = 0.2; // Bounciness (0.0 = inelastic, 1.0 = perfectly elastic) 
                double j = -(1 + restitution) * velocityAlongNormal / totalInvMass;
                Vector2D impulse = normal * j;

                a->setVelocity(a->getVelocity() - impulse * invMassA);
                b->setVelocity(b->getVelocity() + impulse * invMassB);
            }
        }
    }
}