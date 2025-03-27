
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

void PhysicsSystem::handleCollision(std::vector<std::unique_ptr<Entity>> &entityList)
{
    double restitution = 0.2;

    for (size_t i = 0; i < entityList.size(); ++i)
    {
        for (size_t j = i + 1; j < entityList.size(); ++j)
        {
            Entity &a = *entityList[i];
            Entity &b = *entityList[j];

            // Skip if either entity doesn't have a collider
            if (!a.getCollider() || !b.getCollider())
                continue;

            Vector2D normal;
            double penetration = 0;

            if (a.getCollider()->CheckCollision(*b.getCollider(), normal, penetration))
            {

                // Resolve velocities with mass consideration
                double invMassA = a.getInverseMass();
                double invMassB = b.getInverseMass();
                double totalInvMass = invMassA + invMassB;
                if (totalInvMass <= 0)
                    continue; // Both static

                Vector2D relativeVelocity = b.getVelocity() - a.getVelocity(); 
                double velocityAlongNormal = relativeVelocity.dot(normal);
                if (velocityAlongNormal > 0)
                    continue; // Objects are moving away from each other

                double impulseMagnitude = -(1 + restitution) * velocityAlongNormal / totalInvMass;
                Vector2D impulse = normal * impulseMagnitude;

                a.getVelocity() -= impulse * invMassA;
                b.getVelocity() += impulse * invMassB;

                // Positional correction to prevent sinking
                const double slop = 0.01; // Penetration allowance
                const double percent = 0.4; // Correction percentage
                Vector2D correction = normal * std::max(penetration - slop, 0.0) * percent / totalInvMass;

                a.setPosition(a.getPosition() - correction * invMassA);
                b.setPosition(b.getPosition() + correction * invMassB);
            }
        }
    }
}

