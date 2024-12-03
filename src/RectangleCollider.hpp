#ifndef RECTANGLE_COLLIDER_HPP
#define RECTANGLE_COLLIDER_HPP

#include "Collider.hpp"
#include "Vector2D.hpp"
#include <vector>

class CircleCollider; // Forward declaration to avoid circular dependency

class RectangleCollider : public Collider {
public:
    RectangleCollider(const Vector2D center, double width, double height, double rotation = 0.0);

    std::unique_ptr<Collider> clone() const override {
        return std::make_unique<RectangleCollider>(center, width, height);
    }


    bool checkCollision(const Collider& other, Vector2D& collisionNormal, double restitution) const override;
    void resolveCollision(Vector2D& velocity, const Vector2D& collisionNormal, double restitution) const override;

    const Vector2D& getCenter() const { return center; }
    double getWidth() const { return width; }
    double getHeight() const { return height; }
    double getRotation() const { return rotation; } 
    void setRotation(double rotation) override { this->rotation = rotation; }

    std::vector<Vector2D> getVertices() const; 
    std::vector<Vector2D> getAxes() const;
    void projectOntoAxis(const Vector2D& axis, double& min, double& max) const;

private:
    bool projectAndCheckOverlap(const std::vector<Vector2D>& axes, const RectangleCollider& rect, Vector2D& collisionNormal) const;
    bool checkCircleCollision(const CircleCollider& circle, Vector2D& collisionNormal) const;
    Vector2D closestPointOnRectangle(const Vector2D& point) const;

    double width;
    double height;
    double rotation; 
};

#endif // RECTANGLE_COLLIDER_HPP
