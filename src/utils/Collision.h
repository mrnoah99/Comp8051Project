#include <SDL3/SDL_rect.h>

#include "../ecs/Component.h"

class Collision {
    public:
        static bool AABB(const SDL_FRect rectA, SDL_FRect rectB);
        static bool AABB(const Collider& colA, const Collider& colB);
};