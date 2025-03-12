#ifndef CIRCLECOLLIDER_HPP
#define CIRCLECOLLIDER_HPP

#include "Collider.hpp"
#include "RectangleCollider.hpp"

class CircleCollider : public Collider {
private:
    double radius;

public:
    CircleCollider(const Vector2D& center, double radius)
        : Collider(center), radius(radius) {}

    double getRadius() const { return radius; }

    std::unique_ptr<Collider> clone() const override {
        return std::make_unique<CircleCollider>(center, radius);
    }

    bool checkCollision(const Collider& other, Vector2D& collisionNormal, double restitution) const override;
    void resolveCollision(Vector2D& velocity, const Vector2D& collisionNormal, double restitution) const override;

    void projectOntoAxis(const Vector2D& axis, double& min, double& max) const;
};

#endif // CIRCLECOLLIDER_HPP
