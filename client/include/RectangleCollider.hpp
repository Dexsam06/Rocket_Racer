#pragma once
#include "Collider.hpp"

class RectangleCollider : public Collider
{
public:
    RectangleCollider(const Vector2D &position, const Vector2D &size)
        : Collider(position), m_size(size), m_rotation(0) {}

    bool CheckCollision(const Collider &other, Vector2D &normal, double &penetration) const override;
    Vector2D ClosestPoint(const Vector2D &point) const override;
    void ProjectOntoAxis(const Vector2D &axis, double &min, double &max) const override;

    std::vector<Vector2D> GetEdgeNormals() const;
    std::vector<Vector2D> GetVertices() const;

    void updateRotation(double rotation) override {this->m_rotation = rotation; }

    Vector2D m_size;
    double m_rotation;
};