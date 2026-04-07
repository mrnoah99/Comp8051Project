#pragma once

#include <vector>
#include <memory>

#include "../Component.h"

class World;

class RotationSystem {
    public:
        void update(World& world, std::vector<std::unique_ptr<Entity>>& entities, float deltaTime);
    private:
        void applyForce(Transform t, float force) {
            t.rotationForceLeft += force;
        }
};