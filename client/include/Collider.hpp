#pragma once
#include "Vector2D.hpp"
#include <memory>
#include <vector>

class Collider {
protected:
    Vector2D m_position;
    double m_rotation;
    
public:
    Collider(const Vector2D& position, double rotation)  
        : m_position(position), m_rotation(rotation) {}
    virtual ~Collider() = default;

    // Core functionality
    virtual bool CheckCollision(const Collider& other, Vector2D& normal, double& penetration) const = 0;
    virtual void ResolveCollision(Vector2D& velocity, const Vector2D& normal, double restitution, double inverseMass) const = 0;

    // Transform operations
    virtual void UpdatePosition(const Vector2D& newPosition) { m_position = newPosition; }
    virtual void UpdateRotation(double newRotation) { m_rotation = newRotation; }
    
    // Getters
    Vector2D GetPosition() const { return m_position; }
    double GetRotation() const { return m_rotation; }
    
    // Projection for SAT
    virtual void ProjectOntoAxis(const Vector2D& axis, double& min, double& max) const = 0;
};