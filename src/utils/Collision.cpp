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