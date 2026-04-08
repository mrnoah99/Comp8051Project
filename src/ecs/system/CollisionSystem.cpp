#include "CollisionSystem.h"
#include "../Component.h"
#include "../World.h"
#include "../../Game.h"
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

        c.centre = t.position + c.halfSize;
        c.rotation = t.rotation * (SDL_PI_F/180.0f);
        c.right = {
            std::cos(c.rotation),
            std::sin(c.rotation)
        };
        c.forward = {
            -std::sin(c.rotation),
            std::cos(c.rotation)
        };

        c.collisionRotationFactor = 0.0f;
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
                Vector2D localNormal;

                float speedThreshold = 20.0f;

                if (entityA->hasComponent<PlayerTag>() || entityA->hasComponent<Player2Tag>()) {
                    auto& v = entityA->getComponent<Velocity>();
                    auto& c = colliderA; auto& other = colliderB;
                    localNormal = {
                        collisionNormal.dot(colliderA.right),
                        collisionNormal.dot(colliderA.forward)
                    };
                    float impactSpeed = std::abs(v.direction.dot(collisionNormal) * v.speed);

                    if (std::abs(localNormal.x) > std::abs(localNormal.y)) {
                        currentCollisions.insert(makeKey(entityA, entityB));
                        if (localNormal.x > 0) {
                            // right side
                            c.collisionRotationFactor = std::abs(localNormal.x * 1.5f);
                            if (entityA->hasComponent<RightWheels>()) {
                                auto& part = entityA->getComponent<RightWheels>();
                                if (impactSpeed >= speedThreshold) {
                                    part.partHealth.updateHealth(part.partHealth, -1);
                                    if (entityA->hasComponent<Health>()) {
                                        entityA->getComponent<Health>().currentHealth--;
                                    }
                                }
                            }
                        } else {
                            // left side
                            c.collisionRotationFactor = -std::abs(localNormal.x * 1.5f);
                            if (entityA->hasComponent<LeftWheels>()) {
                                auto& part = entityA->getComponent<LeftWheels>();
                                if (impactSpeed >= speedThreshold) {
                                    part.partHealth.updateHealth(part.partHealth, -1);
                                    if (entityA->hasComponent<Health>()) {
                                        entityA->getComponent<Health>().currentHealth--;
                                    }
                                }
                            }
                        }
                    } else {
                        if (localNormal.y > 0) {
                            // front
                            if (entityA->hasComponent<Engine>()) {
                                auto& part = entityA->getComponent<Engine>();
                                if (impactSpeed >= speedThreshold) {
                                    part.partHealth.updateHealth(part.partHealth, -1);
                                    if (entityA->hasComponent<Health>()) {
                                        entityA->getComponent<Health>().currentHealth--;
                                    }
                                }
                            }
                        } else {
                            // back
                            if (entityA->hasComponent<FuelTransmission>()) {
                                auto& part = entityA->getComponent<FuelTransmission>();
                                if (impactSpeed >= speedThreshold) {
                                    part.partHealth.updateHealth(part.partHealth, -1);
                                    if (entityA->hasComponent<Health>()) {
                                        entityA->getComponent<Health>().currentHealth--;
                                    }
                                }
                            }
                        }
                        c.collisionRotationFactor = localNormal.y * localNormal.x;
                    }

                    if (other.tag == "wall" || other.tag == "Wall"
                        || other.tag == "finish" || other.tag == "Finish") other.collisionRotationFactor = 0;
                } else if (entityB->hasComponent<PlayerTag>() || entityB->hasComponent<Player2Tag>()) {
                    auto& v = entityB->getComponent<Velocity>();
                    auto& c = colliderB; auto& other = colliderA;
                    localNormal = {
                        collisionNormal.dot(colliderB.right),
                        collisionNormal.dot(colliderB.forward)
                    };
                    float impactSpeed = std::abs(v.direction.dot(collisionNormal) * v.speed);

                    if (std::abs(localNormal.x) > std::abs(localNormal.y)) {
                        currentCollisions.insert(makeKey(entityB, entityA));
                        if (localNormal.x > 0) {
                            // right side
                            c.collisionRotationFactor = std::abs(localNormal.x * 1.5f);
                            if (entityB->hasComponent<RightWheels>()) {
                                auto& part = entityB->getComponent<RightWheels>();
                                if (impactSpeed >= speedThreshold) {
                                    part.partHealth.updateHealth(part.partHealth, -1);
                                    if (entityB->hasComponent<Health>()) {
                                        entityB->getComponent<Health>().currentHealth--;
                                    }
                                }
                            }
                        } else {
                            // left side
                            c.collisionRotationFactor = -std::abs(localNormal.x * 1.5f);
                            if (entityB->hasComponent<LeftWheels>()) {
                                auto& part = entityB->getComponent<LeftWheels>();
                                if (impactSpeed >= speedThreshold) {
                                    part.partHealth.updateHealth(part.partHealth, -1);
                                    if (entityB->hasComponent<Health>()) {
                                        entityB->getComponent<Health>().currentHealth--;
                                    }
                                }
                            }
                        }
                    } else {
                        if (localNormal.y > 0) {
                            // front
                            if (entityB->hasComponent<Engine>()) {
                                auto& part = entityB->getComponent<Engine>();
                                if (impactSpeed >= speedThreshold) {
                                    part.partHealth.updateHealth(part.partHealth, -1);
                                    if (entityB->hasComponent<Health>()) {
                                        entityB->getComponent<Health>().currentHealth--;
                                    }
                                }
                            }
                        } else {
                            // back
                            if (entityB->hasComponent<FuelTransmission>()) {
                                auto& part = entityB->getComponent<FuelTransmission>();
                                if (impactSpeed >= speedThreshold) {
                                    part.partHealth.updateHealth(part.partHealth, -1);
                                    if (entityB->hasComponent<Health>()) {
                                        entityB->getComponent<Health>().currentHealth--;
                                    }
                                }
                            }
                        }
                        c.collisionRotationFactor = localNormal.y * localNormal.x;
                    }

                    if (other.tag == "wall" || other.tag == "Wall"
                        || other.tag == "finish" || other.tag == "Finish") other.collisionRotationFactor = 0;
                }
            }

            // update health every collision
            // if (entityA->hasComponent<PlayerTag>() && entityA->hasComponent<Health>())
            //     Game::gameState.player1Health = entityA->getComponent<Health>().currentHealth;
            // if (entityA->hasComponent<Player2Tag>() && entityA->hasComponent<Health>())
            //     Game::gameState.player2Health = entityA->getComponent<Health>().currentHealth;

            // if (entityB->hasComponent<PlayerTag>() && entityB->hasComponent<Health>())
            //     Game::gameState.player1Health = entityB->getComponent<Health>().currentHealth;
            // if (entityB->hasComponent<Player2Tag>() && entityB->hasComponent<Health>())
            //     Game::gameState.player2Health = entityB->getComponent<Health>().currentHealth;
        
            
            // check for finish line collisions
            // if (entityA->hasComponent<PlayerTag>() && entityB->hasComponent<FinishLineTag>()) 
            //     Game::gameState.p1Laps++;
            // if (entityA->hasComponent<Player2Tag>() && entityB->hasComponent<FinishLineTag>()) 
            //     Game::gameState.p2Laps++;
            
            // if (entityB->hasComponent<PlayerTag>() && entityA->hasComponent<FinishLineTag>()) 
            //     Game::gameState.p1Laps++;
            // if (entityB->hasComponent<Player2Tag>() && entityA->hasComponent<FinishLineTag>()) 
            //     Game::gameState.p2Laps++;
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