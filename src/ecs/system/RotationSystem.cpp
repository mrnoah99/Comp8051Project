#include "RotationSystem.h"
#include "../World.h"

void RotationSystem::update(World& world, std::vector<std::unique_ptr<Entity>>& entities, float deltaTime) {
    // Rotate entities based on input and collisions
    auto collisionSystem = world.getCollisionSystem();

    for (CollisionKey key : collisionSystem.activeCollisions) {
        Transform t1 = key.first->getComponent<Transform>();
        Transform t2 = key.second->getComponent<Transform>();

        Collider c1 = key.first->getComponent<Collider>();
        Collider c2 = key.second->getComponent<Collider>();

        applyForce(t1, c1.collisionRotationFactor);
        applyForce(t2, c2.collisionRotationFactor);
    }

    for (auto& e : entities) {
        if (e->hasComponent<Transform>() && e->hasComponent<Collider>()) {
            Transform t = e->getComponent<Transform>();

            t.rotation += t.rotationForceLeft * deltaTime;
            t.rotationForceLeft -= t.rotationForceLeft * deltaTime;
        }
    }
}