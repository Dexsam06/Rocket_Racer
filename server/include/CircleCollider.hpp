#pragma once
#include "Collider.hpp"

class CircleCollider : public Collider {
public:
    CircleCollider(const Vector2D& position, double radius)
        : Collider(position), m_radius(radius) {}
        ~CircleCollider() override {}

    bool CheckCollision(const Collider& other, Vector2D& normal, double& penetration) const override;
    Vector2D ClosestPoint(const Vector2D& point) const override;
    void ProjectOntoAxis(const Vector2D& axis, double& min, double& max) const override;

    void updateRotation(double rotation) override {}

    double m_radius;
};  