#pragma once

#include <memory>
#include <vector>

#include "../main.h"

#include "Entity.h"
#include "system/MovementSystem.h"
#include "system/RenderSystem.h"
#include "system/KeyboardInputSystem.h"
#include "system/CollisionSystem.h"
#include "event/EventManager.h"
#include "system/AnimationSystem.h"
#include "system/CameraSystem.h"
#include "system/DestructionSystem.h"
#include "../map.h"
#include "system/EventResponseSystem.h"
#include "../scene/SceneType.h"
#include "system/MainMenuSystem.h"
#include "system/UIRenderSystem.h"
#include "system/RotationSystem.h"
#include "system/CleanupSystem.h"

class World {
    private:
        // map and systems
        Map map;
        std::vector<std::unique_ptr<Entity>> entities;
        std::vector<std::unique_ptr<Entity>> deferredEntities;
        MovementSystem movementSystem;
        RenderSystem renderSystem;
        KeyboardInputSystem keyboardInputSystem;
        CollisionSystem collisionSystem;
        AnimationSystem animationSystem;
        CameraSystem cameraSystem;
        DestructionSystem destructionSystem;
        EventManager eventManager;
        EventResponseSystem eventResponseSystem{*this};
        MainMenuSystem mainMenuSystem;
        UIRenderSystem uiRenderSystem;
        RotationSystem rotationSystem;
        CleanupSystem cleanupSystem;
    public:
        World() = default;

        bool player2 = false;

        // update game state
        void update(float deltaTime, SDL_Event event, SceneType sceneType) {

            if (sceneType == SceneType::MainMenu) {
                mainMenuSystem.update(event);
            } else {
                keyboardInputSystem.update(entities); // keyboard input
                collisionSystem.update(*this); // calculate collisions
                rotationSystem.update(*this, entities, deltaTime); // update rotations
                movementSystem.update(entities, deltaTime); // update movement
                animationSystem.update(entities, deltaTime); // update animations (unused)
                cameraSystem.update(entities, player2); // update camera position
                destructionSystem.update(entities); // update entity health
                cleanupSystem.update(entities, *this); // clean up dead or out-of-bounds entities
            }

            // lifecycle functions
            synchroniseEntities();
            cleanup();
        }

        // draw entities to the screen
        void render() {
            for (auto& e : entities) {
                if (e->hasComponent<Camera>()) {
                    map.draw(e->getComponent<Camera>());
                    break;
                }
            }
            renderSystem.render(entities);
            uiRenderSystem.render(entities);
        }

        void setMovementBoundary() {
            movementSystem.updateWorldSize(map.width * 128.0f, map.height * 128.0f);
        }

        // create new entity with no components
        Entity& createEntity() {
            //emplace back instead of push so we don't create a copy
            entities.emplace_back(std::make_unique<Entity>());
            Main::errors.emplace_back("Entity created.\n");
            return *entities.back();
        }

        // create new entity with no components in time-safe way
        Entity& createDeferredEntity() {
            deferredEntities.emplace_back(std::make_unique<Entity>());
            return *deferredEntities.back();
        }

        std::vector<std::unique_ptr<Entity>>& getEntities() {
            return entities;
        }

        // delete all inactive entities
        void cleanup() {
            //use lambda predicate to remove all inactive entities;
            std::erase_if(
                entities,
                [](std::unique_ptr<Entity>& e) {
                    return !e->isActive();
                }
            );
        }

        // add deferred entities into the main list
        void synchroniseEntities() {
            if (!deferredEntities.empty()) {
                std::move(deferredEntities.begin(), deferredEntities.end(), std::back_inserter(entities));
            }
            deferredEntities.clear();
        }

        EventManager& getEventManager() {return eventManager;}

        CollisionSystem& getCollisionSystem() {return collisionSystem;}

        Map& getMap() {return map;}
};