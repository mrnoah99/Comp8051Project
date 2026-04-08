#include "RotationSystem.h"
#include "../World.h"

void RotationSystem::update(World& world, std::vector<std::unique_ptr<Entity>>& entities, float deltaTime) {
    // Rotate entities based on input and collisions
    auto collisionSystem = world.getCollisionSystem();

    for (CollisionKey key : collisionSystem.activeCollisions) {
        auto& t1 = key.first->getComponent<Transform>();
        auto& t2 = key.second->getComponent<Transform>();

        auto& c1 = key.first->getComponent<Collider>();
        auto& c2 = key.second->getComponent<Collider>();

        if (key.first->hasComponent<PlayerTag>() || key.first->hasComponent<Player2Tag>()
            || key.second->hasComponent<PlayerTag>() || key.second->hasComponent<Player2Tag>()) {
            applyForce(t1, c1.collisionRotationFactor);
            applyForce(t2, c2.collisionRotationFactor);
        }
    }

    for (auto& e : entities) {
        if (e->hasComponent<Transform>() && e->hasComponent<Collider>()) {
            auto& t = e->getComponent<Transform>();

            t.rotation += t.rotationForceLeft * deltaTime;
            t.rotationForceLeft -= t.rotationForceLeft * deltaTime;
        }
        if (e->hasComponent<Transform>() && e->hasComponent<Velocity>()) {
            auto& t = e->getComponent<Transform>();
            auto& v = e->getComponent<Velocity>();

            // convert rotation into desired direction vector
            float rad = t.rotation * (SDL_PI_F/180.0f);
            Vector2D desiredDirection = Vector2D(std::sin(rad), -std::cos(rad));
            desiredDirection.normalize();

            // smoothly rotate over time
            float rotateSpeed = 2.0f;
            v.direction = (v.direction * (1.0f - rotateSpeed * deltaTime)) + (desiredDirection * (rotateSpeed * deltaTime));
        }
    }
}