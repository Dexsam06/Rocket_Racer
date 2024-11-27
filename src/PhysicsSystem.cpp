
#include "PhysicsSystem.hpp"

std::vector<Vector2D> PhysicsSystem::update(std::vector<Entity *> &entityList, double deltaTime)
{
    applyGravity(entityList, deltaTime);
    handleCollision(entityList);
    return calculateFuturePath(entityList, deltaTime, 10);
}

void PhysicsSystem::applyGravity(std::vector<Entity *> &entityList, double deltaTime)
{
    size_t n = entityList.size();
    std::vector<std::pair<double, double>> forces(n, {0.0, 0.0});

    for (size_t i = 0; i < n; ++i)
    {
        for (size_t j = i + 1; j < n; ++j)
        {
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

    for (size_t i = 0; i < n; ++i)
    {
        entityList[i]->update(forces[i].first, forces[i].second, deltaTime);
    }
}

void PhysicsSystem::handleCollision(std::vector<Entity *> &entityList)
{
    double restitution = 0.2;
    for (size_t i = 0; i < entityList.size(); ++i)
    {
        for (size_t j = i + 1; j < entityList.size(); ++j)
        {
            if (i == j)
                continue;
            Vector2D collisionNormal;
            if (entityList[i]->checkCollision(*entityList[j], collisionNormal, restitution))
            {
                entityList[i]->getCollider()->resolveCollision(entityList[i]->getVelocity(), collisionNormal, restitution);
                entityList[j]->getCollider()->resolveCollision(entityList[j]->getVelocity(), -collisionNormal, restitution);
            }
        }
    }
}

std::vector<Vector2D> PhysicsSystem::calculateFuturePath(std::vector<Entity *> &entityList, double deltaTime, double predictionTime)
{
    int steps = static_cast<int>(predictionTime / deltaTime);
    std::vector<Vector2D> futurePath;

    std::vector<std::vector<Vector2D>> temporaryEntityList;
    for (size_t i = 0; i < entityList.size(); i++)
    {
        std::vector<Vector2D> temporaryEntity;
        Vector2D currentPosition = entityList[i]->getPosition();
        Vector2D currentAcceleration = entityList[i]->getAcceleration();
        Vector2D currentVelocity = entityList[i]->getVelocity();
        temporaryEntity.push_back(currentAcceleration);
        temporaryEntity.push_back(currentPosition);
        temporaryEntity.push_back(currentVelocity);
        temporaryEntityList.push_back(temporaryEntity);
    }

    size_t n = temporaryEntityList.size();

    for (int i = 0; i < steps; ++i)
    {
        std::vector<std::pair<double, double>> forces(n, {0.0, 0.0});

        for (size_t i = 0; i < n; ++i)
        {
            for (size_t j = i + 1; j < n; ++j)
            {
                double dx = temporaryEntityList[j][1].x - temporaryEntityList[i][1].x;
                double dy = temporaryEntityList[j][1].y - temporaryEntityList[i][1].y;
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

        for (size_t j = 0; j < n; j++)
        {
            temporaryEntityList[j][0].x = Physics::acceleration(forces[j].first, entityList[j]->getMass());
            temporaryEntityList[j][0].y = Physics::acceleration(forces[j].second, entityList[j]->getMass());

            temporaryEntityList[j][1].x += Physics::distance(temporaryEntityList[j][2].x, temporaryEntityList[j][0].x, deltaTime);
            temporaryEntityList[j][1].y += Physics::distance(temporaryEntityList[j][2].y, temporaryEntityList[j][0].y, deltaTime);

            temporaryEntityList[j][2].x += Physics::velocity(temporaryEntityList[j][0].x, deltaTime);
            temporaryEntityList[j][2].y += Physics::velocity(temporaryEntityList[j][0].y, deltaTime);
        }
        futurePath.push_back(temporaryEntityList[0][1]);
    }

    return futurePath;
}
