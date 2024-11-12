#include "CircleCollider.hpp"
#include "RectangleCollider.hpp"
#include <cmath>
#include <iostream>

CircleCollider::CircleCollider(const Vector2D center, double radius) : Collider(center), radius(radius) {}

bool CircleCollider::checkCollision(const Collider& other, Vector2D& collisionNormal, double restitution) const {
    const CircleCollider* circle = dynamic_cast<const CircleCollider*>(&other); 
    if (circle) {
        Vector2D distanceVec = circle->getCenter() - center; 
        double distance = distanceVec.magnitude(); 
        double radiusSum = radius + circle->getRadius(); 
        double epsilon = 1e-6;
        if (distance < radiusSum + epsilon) {
            collisionNormal = distanceVec.normalized();  // Set the collision normal 
            return true; 
        }
        return false; 
    }
    
    const RectangleCollider* rect = dynamic_cast<const RectangleCollider*>(&other);
    if (rect) {
        return rect->checkCollision(*this, collisionNormal, restitution); // Delegate to rectangle's handling
    }
    
    return false;
}

void CircleCollider::resolveCollision(Vector2D& velocity, const Vector2D& collisionNormal, double restitution) const { 
    Vector2D relativeVelocityAlongNormal = velocity.dot(collisionNormal) * collisionNormal;
    velocity -= (1 + restitution) * relativeVelocityAlongNormal;
} 

void CircleCollider::projectOntoAxis(const Vector2D& axis, double& min, double& max) const { 
    double centerProjection = center.dot(axis);
    min = centerProjection - radius;
    max = centerProjection + radius;
}
