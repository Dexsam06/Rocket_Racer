#include "Entity.hpp"

Entity::Entity(std::unique_ptr<Collider> collider, Vector2D pos, Vector2D vel, double mass)
    : collider(std::move(collider)), position(pos), velocity(vel), mass(mass) {
    }

bool Entity::checkCollision(const Entity& other, Vector2D& collisionNormal, double restitution) const {
    return collider->checkCollision(*other.collider, collisionNormal, restitution);
}

