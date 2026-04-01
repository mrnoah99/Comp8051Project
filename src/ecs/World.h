#pragma once

#include <memory>
#include <vector>

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

        void update(float deltaTime, SDL_Event event, SceneType sceneType) {

            if (sceneType == SceneType::MainMenu) {
                mainMenuSystem.update(event);
            } else {
                keyboardInputSystem.update(entities, event);
                movementSystem.update(entities, deltaTime);
                collisionSystem.update(*this);
                animationSystem.update(entities, deltaTime);
                cameraSystem.update(entities);
                destructionSystem.update(entities);
                rotationSystem.update(*this, entities, deltaTime);
                cleanupSystem.update(entities, *this);
            }

            synchroniseEntities();
            cleanup();
        }

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
            movementSystem.updateWorldSize(map.width * 32.0f, map.height * 32.0f);
        }

        Entity& createEntity() {
            //emplace back instead of push so we don't create a copy
            entities.emplace_back(std::make_unique<Entity>());
            return *entities.back();
        }

        Entity& createDeferredEntity() {
            deferredEntities.emplace_back(std::make_unique<Entity>());
            return *deferredEntities.back();
        }

        std::vector<std::unique_ptr<Entity>>& getEntities() {
            return entities;
        }

        void cleanup() {
            //use lambda predicate to remove all inactive entities;
            std::erase_if(
                entities,
                [](std::unique_ptr<Entity>& e) {
                    return !e->isActive();
                }
            );
        }

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