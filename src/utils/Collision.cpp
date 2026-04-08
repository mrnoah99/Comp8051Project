#include <SDL3/SDL_rect.h>

#include "Collision.h"

bool Collision::AABB(const SDL_FRect rectA, const SDL_FRect rectB) {
    if (rectA.x + rectA.w >= rectB.x &&
        rectB.x + rectB.w >= rectA.x &&
        rectA.y + rectA.h >= rectB.y &&
        rectB.y + rectB.h >= rectA.y) {
            return true;
        } else {
            return false;
        }
}

bool Collision::AABB(const Collider& colA, const Collider& colB) {
    return AABB(colA.rect, colB.rect);
}

// finds collisions between rotatable colliders
bool Collision::OBBvsOBB(const Collider& A, const Collider& B, Vector2D& normal, float& depth) {
    Vector2D axes[4] = {
        A.right, A.forward,
        B.right, B.forward
    };

    float minOverlap = 999999.0f;
    Vector2D smallestAxis;

    Vector2D d = B.centre - A.centre;

    for (int i = 0; i < 4; i++) {
        Vector2D axis = axes[i].normalize();

        float rA = projectOBB(A, axis);
        float rB = projectOBB(B, axis);

        float distance = std::abs(d.dot(axis));
        float overlap = rA + rB - distance;

        if (overlap <= 4.0f)
            return false;

        if (overlap < minOverlap) {
            minOverlap = overlap;
            smallestAxis = axis;
        }
    }

    // Fix direction
    if (d.dot(smallestAxis) < 0)
        smallestAxis = -smallestAxis;

    normal = smallestAxis;
    depth = minOverlap;

    return true;
}

float Collision::projectOBB(const Collider& box, const Vector2D& axis) {
    Vector2D right = box.right;
    Vector2D forward = box.forward;

    return 
        box.halfSize.x * std::abs(right.dot(axis)) +
        box.halfSize.y * std::abs(forward.dot(axis));
}
