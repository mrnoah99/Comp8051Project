#pragma once

#include <vector>
#include <memory>
#include <algorithm>
#include <set>

#include "../Entity.h"

using CollisionKey = std::pair<Entity*, Entity*>;

class World;

class CollisionSystem {
    public:
        void update(World &world);
        std::set<CollisionKey> activeCollisions;
    private:
        std::vector<Entity*> queryCollidables(const std::vector<std::unique_ptr<Entity>>& entities);
        CollisionKey makeKey(Entity* entA, Entity* entB) {
            return std::minmax(entA, entB);
        }
    };