#include "../include/Entity.hpp"

Entity::Entity(std::unique_ptr<Collider> collider, SDL_Texture *texture, Vector2D pos, Vector2D vel, double mass, enet_uint32 ID)
    : collider(std::move(collider)), texture(texture), position(pos), velocity(vel), mass(mass), rotation(0), ID(ID) {
    }

bool Entity::checkCollision(const Entity& other, Vector2D& collisionNormal, double restitution) const {
    return collider->checkCollision(*other.collider, collisionNormal, restitution);
}

