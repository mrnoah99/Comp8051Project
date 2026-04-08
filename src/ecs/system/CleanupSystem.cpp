#include "CleanupSystem.h"
#include "../World.h"

void CleanupSystem::update(std::vector<std::unique_ptr<Entity>>& entities, World& world) {
    for (auto& e : entities) {
        if (e->hasComponent<Transform>()) {
            Transform t = e->getComponent<Transform>();
            if (t.position.x < 0 || t.position.y < 0) {
                e->destroy();
            }
            mapW = world.getMap().width * 128.0f;
            mapH = world.getMap().height * 128.0f;
            if (t.position.x > mapW || t.position.y > mapH) {
                e->destroy();
            }
        }
        if (e->hasComponent<Health>()) {
            Health h = e->getComponent<Health>();
            if (h.currentHealth <= 0) e->destroy();
        }
    }
}