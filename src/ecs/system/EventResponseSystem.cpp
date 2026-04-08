#include "EventResponseSystem.h"
#include "../World.h"
#include "../../Game.h"

EventResponseSystem::EventResponseSystem(World &world) {
    world.getEventManager().subscribe([this, &world](const BaseEvent& e) {
        if (e.type != EventType::Collision) return;
        const auto& collision = static_cast<const CollisionEvent&>(e);

        onCollision(collision, "item", world);
        onCollision(collision, "wall", world);
        onCollision(collision, "projectile", world);
    });

    world.getEventManager().subscribe([this, &world](const BaseEvent& e) {
        if (e.type != EventType::PlayerAction) return;
        const auto& collision = static_cast<const PlayerActionEvent&>(e);

        //TODO onPlayerAction
    });

    world.getEventManager().subscribe([this, &world](const BaseEvent& e) {
        if (e.type != EventType::MouseInteraction) return;
        const auto& mouseEvent = static_cast<const MouseInteractionEvent&>(e);

        onMouseInteraction(mouseEvent);
    });
};

void EventResponseSystem::onCollision(const CollisionEvent& e, const char* otherTag, World& world) {
    Entity* player = nullptr;
    Entity* other = nullptr;

    if (!getCollisionEntities(e, otherTag, player, other)) return;

    if (std::string(otherTag) == "item") {

        if (e.state != CollisionState::Enter) return;

        other->destroy();

        for (auto& e : world.getEntities()) {
            if (!e->hasComponent<SceneState>()) continue;

            auto& sceneState = e->getComponent<SceneState>();
            sceneState.coinsCollected++;
            if (sceneState.coinsCollected > 1) {
                Game::onSceneChangeRequest("level2");
            }
        }
    } else if (std::string(otherTag) == "wall") {

        if (e.state != CollisionState::Stay) return;

        auto& t = player->getComponent<Transform>();
        t.position = t.prevPosition;
    } else if (std::string(otherTag) == "projectile") {

            if (e.state != CollisionState::Enter) return;

            auto& health = player->getComponent<Health>();
            health.currentHealth--;

            Game::gameState.player1Health = health.currentHealth;

            if (health.currentHealth <= 0) {
                player->destroy();
                Game::onSceneChangeRequest("gameover");
            }
    }
}

bool EventResponseSystem::getCollisionEntities(const CollisionEvent& e, const char* otherTag, Entity *&player, Entity *&other) {
    if (e.entA == nullptr || e.entB == nullptr) return false;

    if (!(e.entA->hasComponent<Collider>() && e.entB->hasComponent<Collider>())) return false;

    auto& colliderA = e.entA->getComponent<Collider>();
    auto& colliderB = e.entB->getComponent<Collider>();

    if (colliderA.tag == "player" && colliderB.tag == otherTag) {
        player = e.entA;
        other = e.entB;
    }
    if (colliderA.tag == otherTag && colliderB.tag == "player") {
        player = e.entB;
        other = e.entA;
    }

    return player && other;
}

void EventResponseSystem::onMouseInteraction(const MouseInteractionEvent& e) {
    if (!e.entity->hasComponent<Clickable>()) return;

    auto& clickable = e.entity->getComponent<Clickable>();

    switch (e.state) {
        case MouseInteractionState::Pressed:
            clickable.onPressed();
            break;
        case MouseInteractionState::Released:
            clickable.onReleased();
            break;
        case MouseInteractionState::Cancel:
            clickable.onCancel();
            break;
        default:
            break;
    }
}