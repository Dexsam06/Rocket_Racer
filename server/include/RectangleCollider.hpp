#ifndef RECTANGLECOLLIDER_HPP
#define RECTANGLECOLLIDER_HPP

#include "Collider.hpp"
#include <vector>

class CircleCollider;

class RectangleCollider : public Collider {
private:
    double width, height, rotation; // Rotation in radians

public:
    RectangleCollider(const Vector2D& center, double width, double height)
        : Collider(center), width(width), height(height), rotation(0) {}

    std::vector<Vector2D> getVertices() const;
    std::vector<Vector2D> getAxes() const;

    std::unique_ptr<Collider> clone() const override {
        return std::make_unique<RectangleCollider>(center, width, height);
    }
    void projectOntoAxis(const Vector2D& axis, double& min, double& max) const;
    bool checkCollision(const Collider& other, Vector2D& collisionNormal, double restitution) const override;
    void resolveCollision(Vector2D& velocity, const Vector2D& collisionNormal, double restitution) const override;

    bool projectAndCheckOverlap(const std::vector<Vector2D>& axes, const RectangleCollider& rect, Vector2D& collisionNormal) const;
    bool checkCircleCollision(const CircleCollider& circle, Vector2D& collisionNormal) const;
    Vector2D closestPointOnRectangle(const Vector2D& point) const;
};

#endif // RECTANGLECOLLIDER_HPP
