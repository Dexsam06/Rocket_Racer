#include "../include/CircleCollider.hpp"
#include "../include/RectangleCollider.hpp"
#include <cmath>

bool CircleCollider::CheckCollision(const Collider& other, Vector2D& normal, double& penetration) const {
    if (const CircleCollider* circle = dynamic_cast<const CircleCollider*>(&other)) {
        Vector2D delta = circle->m_position - m_position;
        double distance = delta.magnitude();
        double totalRadius = m_radius + circle->m_radius;

        if (distance >= totalRadius) return false;
        
        normal = distance > 0 ? delta.normalized() : Vector2D(1, 0);
        penetration = totalRadius - distance;
        return true;
    }

    if (const RectangleCollider* rect = dynamic_cast<const RectangleCollider*>(&other)) {
        Vector2D closestPoint = rect->ClosestPoint(m_position);
        Vector2D delta = closestPoint - m_position;
        double distanceSquared = delta.dot(delta);
        
        if (distanceSquared >= m_radius * m_radius) return false;
        
        double distance = std::sqrt(distanceSquared);
        normal = distance > 0 ? delta.normalized() : Vector2D(1, 0);
        penetration = m_radius - distance;
        return true;
    }

    return false;
}

Vector2D CircleCollider::ClosestPoint(const Vector2D& point) const {
    Vector2D direction = point - m_position;
    return m_position + direction.normalized() * m_radius;
}

void CircleCollider::ProjectOntoAxis(const Vector2D& axis, double& min, double& max) const {
    double projection = m_position.dot(axis);
    min = projection - m_radius;
    max = projection + m_radius;
}