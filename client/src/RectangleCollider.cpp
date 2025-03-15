#include "../include/RectangleCollider.hpp"
#include "../include/CircleCollider.hpp"
#include <cmath>
#include <algorithm>
#include <limits>

std::vector<Vector2D> RectangleCollider::getVertices() const {
    double halfWidth = width / 2;
    double halfHeight = height / 2;

    double cosTheta = std::cos(rotation);
    double sinTheta = std::sin(rotation);

    return {
        center + Vector2D(halfWidth * cosTheta - halfHeight * sinTheta, halfWidth * sinTheta + halfHeight * cosTheta),
        center + Vector2D(-halfWidth * cosTheta - halfHeight * sinTheta, -halfWidth * sinTheta + halfHeight * cosTheta),
        center + Vector2D(-halfWidth * cosTheta + halfHeight * sinTheta, -halfWidth * sinTheta - halfHeight * cosTheta),
        center + Vector2D(halfWidth * cosTheta + halfHeight * sinTheta, halfWidth * sinTheta - halfHeight * cosTheta)
    };
}

std::vector<Vector2D> RectangleCollider::getAxes() const { 
    std::vector<Vector2D> vertices = getVertices();
    std::vector<Vector2D> axes;

    for (size_t i = 0; i < vertices.size(); ++i) {
        Vector2D edge = vertices[(i + 1) % vertices.size()] - vertices[i]; 
        if (edge.magnitude() > 1e-6) {
            axes.push_back(edge.perpendicular().normalized());
        }
    }
    return axes;
}

void RectangleCollider::projectOntoAxis(const Vector2D& axis, double& min, double& max) const {
    std::vector<Vector2D> vertices = getVertices();
    min = max = vertices[0].dot(axis);
    for (const auto& vertex : vertices) {
        double projection = vertex.dot(axis);
        if (projection < min) min = projection;
        if (projection > max) max = projection;
    }
}

Vector2D RectangleCollider::closestPointOnRectangle(const Vector2D& point) const {
    Vector2D localPoint = point - center;

    double cosTheta = std::cos(rotation);
    double sinTheta = std::sin(rotation);

    double rotatedX = localPoint.x * cosTheta + localPoint.y * sinTheta;
    double rotatedY = localPoint.y * cosTheta - localPoint.x * sinTheta;

    double clampedX = std::clamp(rotatedX, -width / 2, width / 2);
    double clampedY = std::clamp(rotatedY, -height / 2, height / 2);

    return center + Vector2D(
        clampedX * cosTheta - clampedY * sinTheta,
        clampedX * sinTheta + clampedY * cosTheta
    );
}

bool RectangleCollider::checkCircleCollision(const CircleCollider& circle, Vector2D& collisionNormal) const {
    Vector2D closestPoint = closestPointOnRectangle(circle.getCenter());
    Vector2D circleToRect = closestPoint - circle.getCenter();

    if (circleToRect.magnitude() <= circle.getRadius()) {
        collisionNormal = circleToRect.normalized();
        return true;
    }

    return false;
}

bool RectangleCollider::checkCollision(const Collider& other, Vector2D& collisionNormal, double restitution) const {
    const RectangleCollider* rect = dynamic_cast<const RectangleCollider*>(&other);
    if (rect) {
        std::vector<Vector2D> axes = getAxes();
        std::vector<Vector2D> otherAxes = rect->getAxes();
        axes.insert(axes.end(), otherAxes.begin(), otherAxes.end());
        for (const auto& axis : axes) {
            double minA, maxA, minB, maxB;
            projectOntoAxis(axis, minA, maxA);
            rect->projectOntoAxis(axis, minB, maxB); 

            if (std::min(maxA, maxB) - std::max(minA, minB) <= 0) {
                return false;
            }
        }

        collisionNormal = (rect->getCenter() - center).normalized();
        return true;
    }

    const CircleCollider* circle = dynamic_cast<const CircleCollider*>(&other);
    if (circle) {
        return checkCircleCollision(*circle, collisionNormal);
    }

    return false;
}

void RectangleCollider::resolveCollision(Vector2D& velocity, const Vector2D& collisionNormal, double restitution) const {
    Vector2D relativeVelocityAlongNormal = velocity.dot(collisionNormal) * collisionNormal;
    velocity -= (1 + restitution) * relativeVelocityAlongNormal;
}
