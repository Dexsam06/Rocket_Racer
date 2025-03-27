#include "../include/Collider.hpp"
#include "../include/RectangleCollider.hpp"
#include "../include/CircleCollider.hpp"
#include <cmath>
#include <algorithm>

//==============================
// Circle Collider Implementation
//==============================

bool CircleCollider::CheckCollision(const Collider& other, Vector2D& normal, double& penetration) const {
    // Circle-Circle collision
    if(const CircleCollider* circle = dynamic_cast<const CircleCollider*>(&other)) {
        // For circles, m_position is the center
        const Vector2D delta = circle->m_position - m_position;
        const double distance = delta.magnitude();
        const double totalRadius = m_radius + circle->m_radius; 

        if(distance >= totalRadius) return false;
        
        // Ensure normal points from this circle to the other circle 
        if (distance > 0) {
            normal = delta.normalized();
        } else {
            // If centers are at the same position, use arbitrary direction
            normal = Vector2D(1, 0);
        }
        penetration = totalRadius - distance;
        return true;
    }
    
    // Circle-Rectangle collision
    if(const RectangleCollider* rect = dynamic_cast<const RectangleCollider*>(&other)) {
        // For rectangles, m_position is the top-left corner
        const Vector2D closestPoint = rect->ClosestPoint(m_position);
        Vector2D delta = closestPoint - m_position;
        const double distanceSquared = delta.dot(delta);
        
        if(distanceSquared >= m_radius * m_radius) return false;
        
        const double distance = std::sqrt(distanceSquared);
        
        // Ensure normal points from circle to rectangle
        if (distance > 0) {
            normal = delta.normalized();
        } else {
            // If centers are at the same position, use arbitrary direction
            normal = Vector2D(1, 0);
        }
        penetration = m_radius - distance;
        return true;
    }
    
    return false; 
}

void CircleCollider::ResolveCollision(Vector2D &velocity, const Vector2D &normal, double restitution, double inverseMass) const
{
    const double velocityAlongNormal = velocity.dot(normal);
    if (velocityAlongNormal > 0)
        return;

    const double impulseMagnitude = -(1 + restitution) * velocityAlongNormal / (inverseMass);
    velocity += normal * impulseMagnitude * inverseMass;
}

void CircleCollider::ProjectOntoAxis(const Vector2D &axis, double &min, double &max) const
{
    // For circles, m_position is the center
    const double projection = m_position.dot(axis);
    min = projection - m_radius;
    max = projection + m_radius;
}

//===============================
// Rectangle Collider Implementation
//===============================

bool RectangleCollider::CheckCollision(const Collider& other, Vector2D& normal, double& penetration) const {
    // Rectangle-Circle collision
    if(const CircleCollider* circle = dynamic_cast<const CircleCollider*>(&other)) {
        const bool collision = circle->CheckCollision(*this, normal, penetration);
        if (collision) {
            // Flip the normal since we want it pointing from rectangle to circle
            normal = -normal;
        }
        return collision;
    }
    
    // Rectangle-Rectangle collision (SAT)
    if(const RectangleCollider* rect = dynamic_cast<const RectangleCollider*>(&other)) {
        std::vector<Vector2D> axes = GetEdgeNormals();
        const auto otherAxes = rect->GetEdgeNormals();
        axes.insert(axes.end(), otherAxes.begin(), otherAxes.end());
        
        penetration = std::numeric_limits<double>::max();
        int minAxis = -1;
        
        for(int i = 0; i < axes.size(); i++) {
            const Vector2D& axis = axes[i];
            double minA, maxA, minB, maxB;
            ProjectOntoAxis(axis, minA, maxA);
            rect->ProjectOntoAxis(axis, minB, maxB);
            
            // Check for separation
            if(maxA < minB || maxB < minA) return false;
            
            // Calculate penetration
            const double overlap = std::min(maxA, maxB) - std::max(minA, minB);
            if(overlap < penetration) {
                penetration = overlap;
                normal = axis;
                minAxis = i;
            }
        }
        
        // Ensure normal points from this rectangle to the other rectangle
        // Calculate centers from top-left positions
        Vector2D thisCenter = m_position;
        Vector2D otherCenter = rect->GetPosition();
        const Vector2D direction = otherCenter - thisCenter;
        
        if(direction.dot(normal) < 0) { 
            normal = -normal;
        }
        
        return true;
    }
    
    return false;
}


Vector2D RectangleCollider::ClosestPoint(const Vector2D &point) const
{
    // Position is now the center - no need to calculate center
    const Vector2D localPoint = point - m_position; // Direct offset from center
    
    const double cosAngle = std::cos(-m_rotation);
    const double sinAngle = std::sin(-m_rotation);

    // Rotate point to local coordinates
    const double rotatedX = localPoint.x * cosAngle - localPoint.y * sinAngle;
    const double rotatedY = localPoint.x * sinAngle + localPoint.y * cosAngle;

    // Clamp to rectangle bounds (now using full size)
    const double halfWidth = m_size.x/2;  // m_size is full width/height
    const double halfHeight = m_size.y/2;
    const double clampedX = std::clamp(rotatedX, -halfWidth, halfWidth);
    const double clampedY = std::clamp(rotatedY, -halfHeight, halfHeight);

    // Rotate back to world coordinates
    const double reverseCos = cosAngle;
    const double reverseSin = -sinAngle;

    return m_position + Vector2D(  // Return from center position
        clampedX * reverseCos - clampedY * reverseSin,
        clampedX * reverseSin + clampedY * reverseCos
    );
}

std::vector<Vector2D> RectangleCollider::GetEdgeNormals() const
{
    const auto vertices = GetVertices();
    std::vector<Vector2D> normals;

    for (size_t i = 0; i < vertices.size(); ++i)
    {
        const Vector2D edge = vertices[(i + 1) % 4] - vertices[i];
        normals.push_back(edge.perpendicular().normalized());
    }

    return normals;
}

void RectangleCollider::ResolveCollision(Vector2D &velocity, const Vector2D &normal, double restitution, double inverseMass) const
{
    const double velocityAlongNormal = velocity.dot(normal);
    if (velocityAlongNormal > 0)
        return;

    const double impulseMagnitude = -(1 + restitution) * velocityAlongNormal / (inverseMass);
    velocity += normal * impulseMagnitude * inverseMass; 
}

void RectangleCollider::ProjectOntoAxis(const Vector2D &axis, double &min, double &max) const
{
    const auto vertices = GetVertices();
    min = max = vertices[0].dot(axis);

    for (const auto &vertex : vertices)
    {
        const double projection = vertex.dot(axis);
        min = std::min(min, projection);
        max = std::max(max, projection);
    }
}

std::vector<Vector2D> RectangleCollider::GetVertices() const
{
    // Directly use m_position as center
    const Vector2D halfSize = m_size * 0.5;
    const double cos = std::cos(m_rotation);
    const double sin = std::sin(m_rotation);

    return {
        // Counter-clockwise vertices from center
        m_position + Vector2D(cos * -halfSize.x - sin * -halfSize.y,
                             sin * -halfSize.x + cos * -halfSize.y),
        m_position + Vector2D(cos * -halfSize.x - sin * halfSize.y,
                             sin * -halfSize.x + cos * halfSize.y),
        m_position + Vector2D(cos * halfSize.x - sin * halfSize.y,
                             sin * halfSize.x + cos * halfSize.y),
        m_position + Vector2D(cos * halfSize.x - sin * -halfSize.y,
                             sin * halfSize.x + cos * -halfSize.y)
    };
}