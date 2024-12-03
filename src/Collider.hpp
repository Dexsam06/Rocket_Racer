
#ifndef COLLIDER_HPP
#define COLLIDER_HPP

#include "Vector2D.hpp"
#include <memory>

class Collider {
public:
    Collider(Vector2D center) : center(center) {}
    virtual ~Collider() = default;

    virtual std::unique_ptr<Collider> clone() const = 0;

    virtual bool checkCollision(const Collider &other, Vector2D &collisionNormal, double restitution) const = 0;
    virtual void resolveCollision(Vector2D &velocity, const Vector2D &collisionNormal, double restitution) const = 0;
    virtual void setRotation(double rotation) {}

    void updatePosition(Vector2D position) { this->center = position; }
    Vector2D getCenter() const { return center; }

protected:
    Vector2D center;
};

#endif
