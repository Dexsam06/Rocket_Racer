#include "../include/RectangleCollider.hpp"
#include "../include/CircleCollider.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

bool RectangleCollider::CheckCollision(const Collider& other, Vector2D& normal, double& penetration) const {
    if (const CircleCollider* circle = dynamic_cast<const CircleCollider*>(&other)) {
        bool collision = circle->CheckCollision(*this, normal, penetration);
        if (collision) normal = -normal;
        return collision;
    }

    if (const RectangleCollider* rect = dynamic_cast<const RectangleCollider*>(&other)) {
        std::vector<Vector2D> axes = GetEdgeNormals();
        const auto otherAxes = rect->GetEdgeNormals();
        axes.insert(axes.end(), otherAxes.begin(), otherAxes.end());
        
        penetration = std::numeric_limits<double>::max();
        
        for (const Vector2D& axis : axes) {
            double minA, maxA, minB, maxB;
            ProjectOntoAxis(axis, minA, maxA);
            rect->ProjectOntoAxis(axis, minB, maxB);
            
            if (maxA < minB || maxB < minA) return false;
            
            double overlap = std::min(maxA, maxB) - std::max(minA, minB);
            if (overlap < penetration) {
                penetration = overlap;
                normal = axis;
            }
        }
        
        Vector2D direction = rect->m_position - m_position;
        if (direction.dot(normal) < 0) normal = -normal;
        
        return true;
    }

    return false;
}

Vector2D RectangleCollider::ClosestPoint(const Vector2D& point) const {
    Vector2D localPoint = point - m_position;
    double cosAngle = std::cos(-m_rotation);
    double sinAngle = std::sin(-m_rotation);

    double rotatedX = localPoint.x * cosAngle - localPoint.y * sinAngle;
    double rotatedY = localPoint.x * sinAngle + localPoint.y * cosAngle;

    double halfWidth = m_size.x / 2;
    double halfHeight = m_size.y / 2;
    double clampedX = std::clamp(rotatedX, -halfWidth, halfWidth);
    double clampedY = std::clamp(rotatedY, -halfHeight, halfHeight);

    double reverseCos = std::cos(m_rotation);
    double reverseSin = std::sin(m_rotation);

    return m_position + Vector2D(
        clampedX * reverseCos - clampedY * reverseSin,
        clampedX * reverseSin + clampedY * reverseCos
    );
}

void RectangleCollider::ProjectOntoAxis(const Vector2D& axis, double& min, double& max) const {
    auto vertices = GetVertices();
    min = max = vertices[0].dot(axis);
    for (const auto& vertex : vertices) {
        double projection = vertex.dot(axis);
        min = std::min(min, projection);
        max = std::max(max, projection);
    }
}

std::vector<Vector2D> RectangleCollider::GetEdgeNormals() const {
    auto vertices = GetVertices();
    std::vector<Vector2D> normals;
    for (size_t i = 0; i < vertices.size(); ++i) {
        Vector2D edge = vertices[(i + 1) % 4] - vertices[i];
        normals.push_back(edge.perpendicular().normalized());
    }
    return normals;
}

std::vector<Vector2D> RectangleCollider::GetVertices() const {
    Vector2D halfSize = m_size * 0.5;
    double cos = std::cos(m_rotation);
    double sin = std::sin(m_rotation);

    return {
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