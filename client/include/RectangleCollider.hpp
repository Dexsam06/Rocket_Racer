#pragma once
#include "Collider.hpp"

class RectangleCollider : public Collider {
    Vector2D m_size;
    
public:
    RectangleCollider(const Vector2D& position, const Vector2D& size, double rotation = 0)
        : Collider(position, rotation), m_size(size) {}

    // Core functionality
    bool CheckCollision(const Collider& other, Vector2D& normal, double& penetration) const override;
    void ResolveCollision(Vector2D& velocity, const Vector2D& normal, double restitution, double inverseMass) const override;

    // Projection
    void ProjectOntoAxis(const Vector2D& axis, double& min, double& max) const override;

    // Rectangle-specific
    std::vector<Vector2D> GetVertices() const;
    Vector2D GetSize() const { return m_size; }

    Vector2D ClosestPoint(const Vector2D& point) const;
    std::vector<Vector2D> GetEdgeNormals() const;
};