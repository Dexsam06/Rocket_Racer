#ifndef CIRCLE_COLLIDER_HPP
#define CIRCLE_COLLIDER_HPP

#include "Collider.hpp"
#include "Vector2D.hpp"

class CircleCollider : public Collider {
public:
    CircleCollider(Vector2D center, double radius);

    bool checkCollision(const Collider& other, Vector2D& collisionNormal, double restitution) const override;
    void resolveCollision(Vector2D& velocity, const Vector2D& collisionNormal, double restitution) const override;
    
    double getRadius() const { return radius; }

    void projectOntoAxis(const Vector2D& axis, double& min, double& max) const;
private:
    double radius;
};

#endif // CIRCLE_COLLIDER_HPP
