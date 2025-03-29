#pragma once
#include "Vector2D.hpp"
#include <memory>
#include <vector>

class Collider
{
public:
    Collider(const Vector2D &position)
        : m_position(position) {}
    virtual ~Collider() = default;

    virtual bool CheckCollision(const Collider &other, Vector2D &normal, double &penetration) const = 0;
    virtual Vector2D ClosestPoint(const Vector2D &point) const = 0;
    virtual void ProjectOntoAxis(const Vector2D &axis, double &min, double &max) const = 0;

    void updatePosition(Vector2D position) {m_position = position; } 
    virtual void updateRotation(double rotation) = 0;
    
    Vector2D m_position;
};