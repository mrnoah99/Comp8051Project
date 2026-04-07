#include "CollisionSystem.h"
#include "../Component.h"
#include "../World.h"
#include "../../utils/Collision.h"

#include <cmath>

void CollisionSystem::update(World &world) {
    const std::vector<Entity*> collidables = queryCollidables(world.getEntities());

    //update all positions first
    for (auto entity : collidables) {
        auto& t = entity->getComponent<Transform>();
        auto& c = entity->getComponent<Collider>();
        c.rect.x = t.position.x;
        c.rect.y = t.position.y;

        c.rotation = t.rotation * (SDL_PI_F/180.0f);
        c.right = {
            std::cos(c.rotation),
            std::sin(c.rotation)
        };
        c.forward = {
            -std::sin(c.rotation),
            std::cos(c.rotation)
        };
    }

    std::set<CollisionKey> currentCollisions;

    //check for collisions
    for (size_t i = 0; i < collidables.size(); i++) {
        auto entityA = collidables[i];
        auto& colliderA = entityA->getComponent<Collider>();

        for (size_t j = i + 1; j < collidables.size(); j++) {
            auto entityB = collidables[j];
            auto& colliderB = entityB->getComponent<Collider>();

            if (Collision::AABB(colliderA, colliderB)) {
                CollisionKey key = makeKey(entityA, entityB);
                currentCollisions.insert(key);
                if (!activeCollisions.contains(key)) {
                    world.getEventManager().emit(CollisionEvent{entityA, entityB, CollisionState::Enter});
                }
                world.getEventManager().emit(CollisionEvent{entityA, entityB, CollisionState::Stay});
            }

            Vector2D collisionNormal;
            float depth;
            if (Collision::OBBvsOBB(colliderA, colliderB, collisionNormal, depth)) {
                Vector2D localNormal = {
                    collisionNormal.dot(colliderA.right),
                    collisionNormal.dot(colliderA.forward)
                };

                Velocity v;
                Collider c;
                Collider other;
                if (entityA->hasComponent<PlayerTag>() || entityA->hasComponent<Player2Tag>()) {
                    v = entityA->getComponent<Velocity>();
                    c = colliderA; other = colliderB;
                } else if (entityB->hasComponent<PlayerTag>() || entityB->hasComponent<Player2Tag>()) {
                    v = entityB->getComponent<Velocity>();
                    c = colliderB; other = colliderA;
                }

                if (std::abs(localNormal.x) > std::abs(localNormal.y)) {
                    currentCollisions.insert(makeKey(entityA, entityB));
                    if (localNormal.x > 0) {
                        // right side
                        c.collisionRotationFactor = std::abs(localNormal.x * 180);
                        if (entityA->hasComponent<RightWheels>()) {
                            auto part = entityA->getComponent<RightWheels>();
                            part.partHealth.updateHealth(-1);
                        }
                    } else {
                        // left side
                        c.collisionRotationFactor = -std::abs(localNormal.x * 180);
                        if (entityA->hasComponent<LeftWheels>()) {
                            auto part = entityA->getComponent<LeftWheels>();
                            part.partHealth.updateHealth(-1);
                        }
                    }
                } else {
                    if (localNormal.y > 0) {
                        // front
                        if (entityA->hasComponent<Engine>()) {
                            auto part = entityA->getComponent<Engine>();
                            part.partHealth.updateHealth(-1);
                        }
                    } else {
                        // back
                        if (entityA->hasComponent<FuelTransmission>()) {
                            auto part = entityA->getComponent<FuelTransmission>();
                            part.partHealth.updateHealth(-1);
                        }
                    }
                    c.collisionRotationFactor = localNormal.y * localNormal.x;
                }

                if (other.tag == "wall" || other.tag == "Wall") other.collisionRotationFactor = 0;

                // slow down based on angle of collision (shallower collisions should slow less)
                v.speed -= v.speed * std::abs(std::cos(c.rotation - other.rotation));
            }
        }
    }

    for (auto& key : activeCollisions) {
        if (!currentCollisions.contains(key)) {
            world.getEventManager().emit(CollisionEvent{key.first, key.second, CollisionState::Exit});
        }
    }

    activeCollisions = std::move(currentCollisions);
}

std::vector<Entity*> CollisionSystem::queryCollidables(const std::vector<std::unique_ptr<Entity>>& entities) {
    std::vector<Entity*> collidables;

    for (auto& e : entities) {
        if (e->hasComponent<Transform>() && e->hasComponent<Collider>()) {
            collidables.push_back(e.get());
        }
    }

    return collidables;
}