#include <cmath>

#include "Vector2D.h"

Vector2D Vector2D::operator+(const Vector2D& vec) const {
    return Vector2D(x + vec.x, y + vec.y);
}

Vector2D Vector2D::operator-(const Vector2D& vec) const {
    return Vector2D(x - vec.x, y - vec.y);
}

//member operator function that allows vector2D to be on left side
Vector2D Vector2D::operator*(float scalar) const {
    return Vector2D(x * scalar, y * scalar);
}

//non member operator function that allows vector2D to be on right side
Vector2D operator*(float scalar, Vector2D& vec) {
    return Vector2D(vec.x * scalar, vec.y * scalar);
}

Vector2D Vector2D::operator/(float scalar) const {
    return Vector2D(x / scalar, y / scalar);
}

Vector2D operator/(float scalar, Vector2D& vec) {
    return Vector2D(vec.x / scalar, vec.y / scalar);
}

Vector2D& Vector2D::operator*=(float scalar) {
    this->x *= scalar;
    this->y *= scalar;
    return *this;
}

Vector2D& Vector2D::operator/=(float scalar) {
    this->x /= scalar;
    this->y /= scalar;
    return *this;
}

//member operator function that allows addition of 2 vectors
Vector2D& Vector2D::operator+=(const Vector2D& vec) {
    this->x += vec.x;
    this->y += vec.y;
    return *this;
}

Vector2D& Vector2D::operator-=(const Vector2D& vec) {
    this->x -= vec.x;
    this->y -= vec.y;
    return *this;
}

bool Vector2D::operator==(Vector2D vec) {
    if (x == vec.x && y == vec.y) return true;
    else return false;
}

bool Vector2D::operator!=(Vector2D vec) {
    if (x != vec.x || y != vec.y) return true;
    else return false;
}

Vector2D operator-(Vector2D vec) {
    return Vector2D(-vec.x, -vec.y);
}

Vector2D& Vector2D::normalize() {
    //pythagorean theorem
    float length = std::sqrt(x*x + y*y);

    if (length > 0) {
        this->x /= length;
        this->y /= length;
    }

    return *this;
}

float Vector2D::dot(Vector2D vec) {
    return {x * vec.x + y * vec.y};
}