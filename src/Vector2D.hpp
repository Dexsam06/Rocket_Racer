
#ifndef VECTOR2D_HPP
#define VECTOR2D_HPP

struct Vector2D {
    double x;
    double y;

    Vector2D(double x = 0, double y = 0) : x(x), y(y) {}

    // Vector operations
    Vector2D operator+(const Vector2D& other) const {
        return Vector2D(x + other.x, y + other.y);
    }

    Vector2D operator-(const Vector2D& other) const {
        return Vector2D(x - other.x, y - other.y);
    }
};

#endif
