#ifndef COLLIDER_HPP
#define COLLIDER_HPP

#include "Vector2D.hpp"
#include <memory>

class Collider {
protected:
    Vector2D center;

public:
    Collider(const Vector2D& center) : center(center) {}
    virtual ~Collider() = default;

    Vector2D getCenter() const { return center; }
    void updatePosition(const Vector2D& position) { center = position; }
    virtual void setRotation(double rotation) {}

    virtual std::unique_ptr<Collider> clone() const = 0;
    virtual bool checkCollision(const Collider& other, Vector2D& collisionNormal, double restitution) const = 0;
    virtual void resolveCollision(Vector2D& velocity, const Vector2D& collisionNormal, double restitution) const = 0;
};

#endif // COLLIDER_HPP
