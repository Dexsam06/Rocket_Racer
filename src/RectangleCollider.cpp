#include "RectangleCollider.hpp"
#include "CircleCollider.hpp"
#include <cmath>
#include <algorithm>
#include <limits>
#include <iostream>

RectangleCollider::RectangleCollider(const Vector2D center, double width, double height, double rotation)
    : Collider(center), width(width), height(height), rotation(rotation) {}

std::vector<Vector2D> RectangleCollider::getVertices() const {
    double halfWidth = width / 2;
    double halfHeight = height / 2;

    double cosTheta = std::cos(rotation);
    double sinTheta = std::sin(rotation); 

    return {
        center + Vector2D( halfWidth * cosTheta - halfHeight * sinTheta,  halfWidth * sinTheta + halfHeight * cosTheta),
        center + Vector2D(-halfWidth * cosTheta - halfHeight * sinTheta, -halfWidth * sinTheta + halfHeight * cosTheta),
        center + Vector2D(-halfWidth * cosTheta + halfHeight * sinTheta, -halfWidth * sinTheta - halfHeight * cosTheta),
        center + Vector2D( halfWidth * cosTheta + halfHeight * sinTheta,  halfWidth * sinTheta - halfHeight * cosTheta)
    };
}

std::vector<Vector2D> RectangleCollider::getAxes() const {
    std::vector<Vector2D> vertices = getVertices();
    std::vector<Vector2D> axes;

    for (size_t i = 0; i < vertices.size(); ++i) {
        Vector2D edge = vertices[(i + 1) % vertices.size()] - vertices[i];
        axes.push_back(edge.perpendicular().normalized());
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

bool RectangleCollider::projectAndCheckOverlap(const std::vector<Vector2D>& axes, const RectangleCollider& rect, Vector2D& collisionNormal) const {
    double minOverlap = std::numeric_limits<double>::infinity();
    Vector2D smallestAxis;

    for (const auto& axis : axes) {
        double minA, maxA, minB, maxB;
        projectOntoAxis(axis, minA, maxA);
        rect.projectOntoAxis(axis, minB, maxB);
        double overlap = std::min(maxA, maxB) - std::max(minA, minB);
        if (overlap <= 0) return false;

        if (overlap < minOverlap) {
            minOverlap = overlap;
            smallestAxis = axis;
        }
    }

    collisionNormal = smallestAxis.normalized();
    return true;
}

bool RectangleCollider::checkCircleCollision(const CircleCollider& circle, Vector2D& collisionNormal) const {
    std::vector<Vector2D> axes = getAxes();

    // Additional axis from circle to the closest point on rectangle
    Vector2D closestPoint = closestPointOnRectangle(circle.getCenter());
    axes.push_back((closestPoint - circle.getCenter()).normalized());

    double minOverlap = std::numeric_limits<double>::infinity();
    Vector2D smallestAxis;

    for (const auto& axis : axes) {
        double minA, maxA, minB, maxB;
        projectOntoAxis(axis, minA, maxA);
        circle.projectOntoAxis(axis, minB, maxB);

        double overlap = std::min(maxA, maxB) - std::max(minA, minB);
        if (overlap <= 0) return false;

        if (overlap < minOverlap) {
            minOverlap = overlap;
            smallestAxis = axis;
        }
    }

    collisionNormal = smallestAxis.normalized();
    return true;
}

Vector2D RectangleCollider::closestPointOnRectangle(const Vector2D& point) const {
    Vector2D localPoint = point - center;

    double cosTheta = std::cos(rotation);
    double sinTheta = std::sin(rotation);

    // Rotate point to align with the rectangle's local axis
    double rotatedX = localPoint.x * cosTheta + localPoint.y * sinTheta;
    double rotatedY = localPoint.y * cosTheta - localPoint.x * sinTheta;

    // Clamp to rectangle's bounds
    double clampedX = std::clamp(rotatedX, -width / 2, width / 2);
    double clampedY = std::clamp(rotatedY, -height / 2, height / 2);

    // Rotate back to world coordinates
    return center + Vector2D(
        clampedX * cosTheta - clampedY * sinTheta,
        clampedX * sinTheta + clampedY * cosTheta
    );
}

bool RectangleCollider::checkCollision(const Collider& other, Vector2D& collisionNormal, double restitution) const {
    const RectangleCollider* rect = dynamic_cast<const RectangleCollider*>(&other);
    if (rect) {
        std::vector<Vector2D> axes = getAxes();
        std::vector<Vector2D> otherAxes = rect->getAxes();
        axes.insert(axes.end(), otherAxes.begin(), otherAxes.end());
        if (projectAndCheckOverlap(axes, *rect, collisionNormal)) {
            return true;
        }
    }

    const CircleCollider* circle = dynamic_cast<const CircleCollider*>(&other);
    if (circle) {
        if (checkCircleCollision(*circle, collisionNormal)) {
            return true;
        }
    }

    return false;
}

void RectangleCollider::resolveCollision(Vector2D& velocity, const Vector2D& collisionNormal, double restitution) const {
    Vector2D relativeVelocityAlongNormal = velocity.dot(collisionNormal) * collisionNormal;
    velocity -= (1 + restitution) * relativeVelocityAlongNormal; 
}
