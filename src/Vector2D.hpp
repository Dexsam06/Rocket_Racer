
#ifndef VECTOR2D_HPP
#define VECTOR2D_HPP

#include <cmath>

struct Vector2D {
public:
    double x, y; 

    Vector2D(double x = 0, double y = 0) : x(x), y(y) {}

    double dot(const Vector2D& other) const { return x * other.x + y * other.y; }
    double magnitude() const { return std::sqrt(x * x + y * y); }
    Vector2D normalized() const {double mag = magnitude(); return (mag > 0) ? Vector2D(x / mag, y / mag) : Vector2D(); }
    Vector2D perpendicular() const {return Vector2D(-y, x); } 
    
    friend Vector2D operator*(double scalar, const Vector2D &vec) 
    {
        return Vector2D(vec.x * scalar, vec.y * scalar);
    }

    friend Vector2D operator-(Vector2D &vec) {
        return Vector2D(-vec.x, -vec.y);
    }
  
    Vector2D operator-(const Vector2D& other) const { return Vector2D(x - other.x, y - other.y); }
    Vector2D operator+(const Vector2D& other) const { return Vector2D(x + other.x, y + other.y); }
    Vector2D operator*(double scalar) const { return Vector2D(x * scalar, y * scalar); }

    Vector2D& operator*=(const Vector2D &vec)   { x *= vec.x, y *= vec.y; return *this; }
    Vector2D& operator-=(const Vector2D& other) { x -= other.x; y -= other.y; return *this; }
    Vector2D& operator+=(const Vector2D& other) { x += other.x; y += other.y; return *this; }
};

#endif // VECTOR2D_HPPc



    