#pragma once
#include "Collider.hpp"

class CircleCollider : public Collider {
    double m_radius;
    
public:
    CircleCollider(const Vector2D& position, double radius, double rotation = 0)
        : Collider(position, rotation), m_radius(radius) {}

    // Core functionality
    bool CheckCollision(const Collider& other, Vector2D& normal, double& penetration) const override;
    void ResolveCollision(Vector2D& velocity, const Vector2D& normal, double restitution, double inverseMass) const override;

    // Projection
    void ProjectOntoAxis(const Vector2D& axis, double& min, double& max) const override; 

    // Circle-specific
    double GetRadius() const { return m_radius; }
    void SetRadius(double radius) { m_radius = radius; }
};